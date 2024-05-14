<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="gaco-bulldozer" tilewidth="112" tileheight="80" tilecount="55" columns="11">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="stopGID" type="int" value="10"/>
    <property name="ticks-multiplier" type="float" value="0.5"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="22"/>
    <property name="stopGID" type="int" value="24"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="11"/>
    <property name="stopGID" type="int" value="12"/>
    <property name="ticks-multiplier" type="float" value="2"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="33"/>
    <property name="stopGID" type="int" value="38"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="144"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="44"/>
    <property name="stopGID" type="int" value="47"/>
   </properties>
  </property>
  <property name="inverted" type="bool" value="true"/>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/gaco-bulldozer.png" width="1232" height="400"/>
</tileset>
