<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="gaco-microwave" tilewidth="80" tileheight="80" tilecount="75" columns="15">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="0"/>
    <property name="stopGID" type="int" value="14"/>
    <property name="ticks-multiplier" type="float" value="0.5"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="30"/>
    <property name="stopGID" type="int" value="33"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="15"/>
    <property name="stopGID" type="int" value="22"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="45"/>
    <property name="stopGID" type="int" value="49"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="200"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="60"/>
    <property name="stopGID" type="int" value="65"/>
   </properties>
  </property>
  <property name="inverted" type="bool" value="true"/>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/gaco-microwave.png" width="1200" height="400"/>
 <tile id="14">
  <properties>
   <property name="animation-idle" type="class" propertytype="animation"/>
  </properties>
 </tile>
</tileset>
