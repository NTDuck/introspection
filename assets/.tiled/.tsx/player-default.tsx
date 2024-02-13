<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="default" tilewidth="32" tileheight="32" tilecount="2337" columns="57">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="1482"/>
    <property name="stopGID" type="int" value="1487"/>
   </properties>
  </property>
  <property name="animation-attack-ranged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="2052"/>
    <property name="stopGID" type="int" value="2054"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="animation-update-rate-multiplier" type="float" value="2"/>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="2166"/>
    <property name="stopGID" type="int" value="2168"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="animation-update-rate-multiplier" type="float" value="4"/>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="2166"/>
    <property name="stopGID" type="int" value="2168"/>
   </properties>
  </property>
  <property name="animation-height" type="int" value="2"/>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="114"/>
    <property name="stopGID" type="int" value="119"/>
   </properties>
  </property>
  <property name="animation-run" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="912"/>
    <property name="stopGID" type="int" value="917"/>
   </properties>
  </property>
  <property name="animation-update-rate" type="int" value="10"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="228"/>
    <property name="stopGID" type="int" value="233"/>
   </properties>
  </property>
  <property name="animation-width" type="int" value="1"/>
 </properties>
 <image source="../../graphics/sprites/entities/player/default.png" width="1854" height="1312"/>
</tileset>
