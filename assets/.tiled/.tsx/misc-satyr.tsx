<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="misc-satyr" tilewidth="32" tileheight="32" tilecount="80" columns="10">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="70"/>
    <property name="stopGID" type="int" value="75"/>
    <property name="ticks-multiplier" type="float" value="0.5"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="60"/>
    <property name="stopGID" type="int" value="63"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="50"/>
    <property name="stopGID" type="int" value="59"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="0"/>
    <property name="stopGID" type="int" value="5"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="200"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="10"/>
    <property name="stopGID" type="int" value="17"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/misc-satyr.png" width="320" height="256"/>
</tileset>
