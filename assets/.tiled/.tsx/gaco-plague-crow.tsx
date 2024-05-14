<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="gaco-plague-crow" tilewidth="64" tileheight="64" tilecount="30" columns="6">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="0"/>
    <property name="stopGID" type="int" value="4"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="6"/>
    <property name="stopGID" type="int" value="8"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="12"/>
    <property name="stopGID" type="int" value="17"/>
    <property name="ticks-multiplier" type="float" value="2"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="18"/>
    <property name="stopGID" type="int" value="21"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="200"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="24"/>
    <property name="stopGID" type="int" value="27"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/gaco-plague-crow.png" width="384" height="320"/>
</tileset>
