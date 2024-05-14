<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="misc-chick-boy" tilewidth="32" tileheight="32" tilecount="16" columns="1">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="0"/>
    <property name="stopGID" type="int" value="1"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="7"/>
    <property name="stopGID" type="int" value="10"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="7"/>
    <property name="stopGID" type="int" value="16"/>
    <property name="ticks-multiplier" type="float" value="0.1"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="stopGID" type="int" value="5"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="200"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="6"/>
    <property name="stopGID" type="int" value="15"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/misc-chick-boy.png" width="32" height="512"/>
</tileset>
