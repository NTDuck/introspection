<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="egi-crab" tilewidth="64" tileheight="64" tilecount="36" columns="4">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="12"/>
    <property name="stopGID" type="int" value="22"/>
    <property name="ticks-multiplier" type="float" value="0.25"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="24"/>
    <property name="stopGID" type="int" value="26"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="28"/>
    <property name="stopGID" type="int" value="32"/>
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
    <property name="startGID" type="int" value="4"/>
    <property name="stopGID" type="int" value="9"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/egi-crab.png" width="256" height="576"/>
</tileset>
