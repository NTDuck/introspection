<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="dazw-zombie" tilewidth="73" tileheight="73" tilecount="60" columns="6">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="18"/>
    <property name="stopGID" type="int" value="24"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="30"/>
    <property name="stopGID" type="int" value="38"/>
    <property name="ticks-multiplier" type="float" value="0.5"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="42"/>
    <property name="stopGID" type="int" value="57"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="stopGID" type="int" value="3"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="200"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="6"/>
    <property name="stopGID" type="int" value="13"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/dazw-zombie.png" width="438" height="730"/>
</tileset>
