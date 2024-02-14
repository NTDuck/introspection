<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="player-premade-05" tilewidth="32" tileheight="32" tilecount="2296" columns="56">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="1456"/>
    <property name="stopGID" type="int" value="1461"/>
   </properties>
  </property>
  <property name="animation-attack-ranged" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="2016"/>
    <property name="stopGID" type="int" value="2018"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="animation-update-rate-multiplier" type="float" value="2"/>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="2128"/>
    <property name="stopGID" type="int" value="2130"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="animation-update-rate-multiplier" type="float" value="4"/>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="2128"/>
    <property name="stopGID" type="int" value="2130"/>
   </properties>
  </property>
  <property name="animation-height" type="int" value="2"/>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="112"/>
    <property name="stopGID" type="int" value="117"/>
   </properties>
  </property>
  <property name="animation-run" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="896"/>
    <property name="stopGID" type="int" value="901"/>
   </properties>
  </property>
  <property name="animation-update-rate" type="int" value="10"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="224"/>
    <property name="stopGID" type="int" value="229"/>
   </properties>
  </property>
  <property name="animation-width" type="int" value="1"/>
 </properties>
 <image source="../../graphics/sprites/entities/player/premade-05.png" width="1792" height="1312"/>
</tileset>
