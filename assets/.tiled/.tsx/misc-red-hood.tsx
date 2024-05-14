<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="misc-red-hood" tilewidth="80" tileheight="80" tilecount="104" columns="26">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="52"/>
    <property name="stopGID" type="int" value="77"/>
    <property name="ticks-multiplier" type="float" value="0.01"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="26"/>
    <property name="stopGID" type="int" value="32"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="26"/>
    <property name="stopGID" type="int" value="29"/>
    <property name="ticks-multiplier" type="float" value="2"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="stopGID" type="int" value="17"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="144"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="78"/>
    <property name="stopGID" type="int" value="101"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/misc-red-hood.png" width="2080" height="320"/>
</tileset>
