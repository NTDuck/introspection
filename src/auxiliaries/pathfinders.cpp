#include <auxiliaries.hpp>

#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <vector>
#include <stack>


double pathfinders::Cell::getG(Cell const& cell) {
    return std::sqrt(std::pow(cell.x, 2) + std::pow(cell.y, 2));
}

template <pathfinders::Heuristic H>
typename std::enable_if_t<H == pathfinders::Heuristic::kManhattan, double>
pathfinders::Cell::getH(Cell const& cell, Cell const& dest) {
    return std::abs(cell.x - dest.x) + std::abs(cell.y - dest.y);
}

template <pathfinders::Heuristic H, unsigned short int Dsq, unsigned short int D2sq>
typename std::enable_if_t<H == pathfinders::Heuristic::kDiagonal, double>
pathfinders::Cell::getH(Cell const& cell, Cell const& dest) {
    auto dx = std::abs(cell.x - dest.x);
    auto dy = std::abs(cell.y - dest.y);
    return std::sqrt(Dsq) * std::max(dx, dy) + (std::sqrt(D2sq) - std::sqrt(Dsq)) * std::min(dx, dy);
}

template <pathfinders::Heuristic H>
typename std::enable_if_t<H == pathfinders::Heuristic::kChebyshev, double>
pathfinders::Cell::getH(Cell const& cell, Cell const& dest) {
    return getH<Heuristic::kDiagonal, 1, 1>(cell, dest);
}

template <pathfinders::Heuristic H>
typename std::enable_if_t<H == pathfinders::Heuristic::kOctile, double>
pathfinders::Cell::getH(Cell const& cell, Cell const& dest) {
    return getH<Heuristic::kDiagonal, 1, 2>(cell, dest);
}

template <pathfinders::Heuristic H>
typename std::enable_if_t<H == pathfinders::Heuristic::kEuclidean, double>
pathfinders::Cell::getH(Cell const& cell, Cell const& dest) {
    return std::sqrt(std::pow(cell.x - dest.x, 2) + std::pow(cell.y - dest.y, 2));
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
pathfinders::ASPF<H, M>::ASPF(std::vector<std::vector<int>> const& grid) : mGrid(grid) {
    setBegin({ 0, 0 });
    setEnd({ static_cast<int>(mGrid.front().size()) - 1, static_cast<int>(mGrid.size()) - 1 });
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
bool pathfinders::ASPF<H, M>::isValid(Cell const& cell) const {
    return static_cast<int>(mBegin.x) <= cell.x && cell.x <= static_cast<int>(mEnd.x) && static_cast<int>(mBegin.y) <= cell.y && cell.y <= static_cast<int>(mEnd.y);
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
bool pathfinders::ASPF<H, M>::isUnblocked(Cell const& cell) const {
    return mGrid[cell.y][cell.x] != 0;
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
bool pathfinders::ASPF<H, M>::isUnblocked(Cell const& cell, Cell const& successor) const {
    return mGrid[successor.y][successor.x] != 0;
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
std::stack<pathfinders::Cell> pathfinders::ASPF<H, M>::getPath(std::vector<std::vector<Cell::Data>> const& cellData, Cell const& dest) const {
    std::stack<Cell> path;
    auto curr = dest;

    while (!(cellData[curr.y - mBegin.y][curr.x - mBegin.x].parent == curr)) {
        path.push(curr);
        curr = cellData[curr.y - mBegin.y][curr.x - mBegin.x].parent;
    }

    path.push(curr);
    return path;
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
void pathfinders::ASPF<H, M>::setBegin(Cell const& begin) {
    mBegin = {
        std::max(0, begin.x),
        std::max(0, begin.y),
    };
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
void pathfinders::ASPF<H, M>::setEnd(Cell const& end) {
    mEnd = {
        std::min(static_cast<int>(mGrid.front().size()) - 1, end.x),
        std::min(static_cast<int>(mGrid.size()) - 1, end.y),
    };
}

template <pathfinders::Heuristic H, pathfinders::MovementType M>
pathfinders::Result pathfinders::ASPF<H, M>::search(Cell const& src, Cell const& dest) const {
    if (mGrid.empty()) return Result(Status::kFailure);
    
    if (!isValid(src)) return Result(Status::kInvalidSrc);
    if (!isValid(dest)) return Result(Status::kInvalidDest);
    if (!isUnblocked(src)) return Result(Status::kBlockedSrc);
    if (!isUnblocked(dest)) return Result(Status::kBlockedDest);
    if (src == dest) return Result(Status::kCoincidents);

    // Note that index-based operations on two following lists require substracting `mBegin` to match `mGrid`
    // Initialize closed list
    std::vector<std::vector<bool>> closedList(mEnd.y - mBegin.y + 1, std::vector<bool>(mEnd.x - mBegin.x + 1, false));   // `false` means not visited (and vice versa)

    // Initialize cell data list
    std::vector<std::vector<Cell::Data>> cellDataList(mEnd.y - mBegin.y + 1, std::vector<Cell::Data>(mEnd.x - mBegin.x + 1, Cell::Data{}));
    
    // Initialize starting node parameters
    auto& srcData = cellDataList[src.y - mBegin.y][src.x - mBegin.x];
    srcData.f = 0;
    srcData.parent = src;

    // Initialize open list
    std::stack<Cell> openList;   // For operations at O(1) time complexity
    openList.push(src);   // Place the starting cell on the open list

    double g, h, f;

    while (!openList.empty()) {
        g = h = f = 0;

        auto parent = openList.top();
        openList.pop();   // Remove cell from open list
        closedList[parent.y - mBegin.y][parent.x - mBegin.x] = true;   // Add cell to closed list

        // Generate all successors
        for (const auto& direction : mDirections) {
            auto successor = parent + direction;
            if (!isValid(successor)) continue;

            // If successor is destination, search is considered successful
            if (successor == dest) {
                cellDataList[successor.y - mBegin.y][successor.x - mBegin.x].parent = parent;
                return Result(Status::kSuccess, getPath(cellDataList, dest));
            }

            // Ignore if successor is already on the closed list or is blocked
            if (!closedList[successor.y - mBegin.y][successor.x - mBegin.x] && isUnblocked(successor)) {
                g = cellDataList[parent.y - mBegin.y][parent.x - mBegin.x].g + Cell::getG(direction);
                h = Cell::getH<H>(successor, dest);
                f = g + h;

                auto& successorData = cellDataList[successor.y - mBegin.y][successor.x - mBegin.x];

                // Add successor to the open list if successor is not on the open list or provides a better path
                if (successorData.f == std::numeric_limits<double>::max() || successorData.f > f) {
                    openList.push(successor);

                    // Update successor data
                    successorData.f = f;
                    successorData.g = g;
                    successorData.h = h;
                    successorData.parent = parent;
                }
            }
        }
    }

    // Search is considered unsuccessful if the open list is emptied before destination cell is found
    return Result(Status::kFailure);
}


template class pathfinders::ASPF<pathfinders::Heuristic::kManhattan, pathfinders::MovementType::k4Directional>;
template class pathfinders::ASPF<pathfinders::Heuristic::kChebyshev, pathfinders::MovementType::k4Directional>;
template class pathfinders::ASPF<pathfinders::Heuristic::kOctile, pathfinders::MovementType::k4Directional>;
template class pathfinders::ASPF<pathfinders::Heuristic::kEuclidean, pathfinders::MovementType::k4Directional>;
template class pathfinders::ASPF<pathfinders::Heuristic::kConstantZero, pathfinders::MovementType::k4Directional>;
template class pathfinders::ASPF<pathfinders::Heuristic::kContantInf, pathfinders::MovementType::k4Directional>;