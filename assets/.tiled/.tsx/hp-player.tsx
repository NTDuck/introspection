<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="hp-player" tilewidth="32" tileheight="32" tilecount="72" columns="8" tilerendersize="grid">
 <properties>
  <property name="animation-attack" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="64"/>
    <property name="stopGID" type="int" value="71"/>
    <property name="type" value="attack"/>
   </properties>
  </property>
  <property name="animation-blink" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="8"/>
    <property name="stopGID" type="int" value="9"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="56"/>
    <property name="stopGID" type="int" value="63"/>
   </properties>
  </property>
  <property name="animation-disappear" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="48"/>
    <property name="stopGID" type="int" value="51"/>
   </properties>
  </property>
  <property name="animation-duck" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="32"/>
    <property name="stopGID" type="int" value="37"/>
   </properties>
  </property>
  <property name="animation-height" type="int" value="1"/>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="isDefault" type="bool" value="true"/>
    <property name="startGID" type="int" value="0"/>
    <property name="stopGID" type="int" value="1"/>
   </properties>
  </property>
  <property name="animation-jump" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="40"/>
    <property name="stopGID" type="int" value="47"/>
   </properties>
  </property>
  <property name="animation-run" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="24"/>
    <property name="stopGID" type="int" value="31"/>
   </properties>
  </property>
  <property name="animation-update-rate" type="int" value="64"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="16"/>
    <property name="stopGID" type="int" value="19"/>
   </properties>
  </property>
  <property name="animation-width" type="int" value="1"/>
 </properties>
 <image source="../../graphics/sprites/player/hooded/player.png" width="256" height="288"/>
</tileset>
