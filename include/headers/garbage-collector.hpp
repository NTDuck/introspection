#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stack>


class GarbageCollector final {
    class BaseWrapper {
        public:
            virtual ~BaseWrapper() = default;
            virtual void terminate() = 0;
    };

    public:
        template <typename T>
        class Wrapper : public BaseWrapper {
            public:
                inline Wrapper(T* instance) : mInstance(instance) {}
                inline virtual ~Wrapper() override { delete mInstance; }
                inline virtual void terminate() override { delete this; }

            private:
                T* mInstance;
        };
        
        GarbageCollector() = default;
        ~GarbageCollector() = default;

        template <typename T>
        inline void insert(T* instance) {
            mInstances.push(new Wrapper<T>(instance));
        }

        inline void clear() {
            while (!mInstances.empty()) {
                mInstances.top()->terminate();
                mInstances.pop();
            }
        }

    private:
        std::stack<BaseWrapper*> mInstances;
};


#endif