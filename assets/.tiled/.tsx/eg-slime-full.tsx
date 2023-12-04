<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="slime-full" tilewidth="64" tileheight="64" tilecount="30" columns="6">
 <properties>
  <property name="animation-attack" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="16"/>
    <property name="stopGID" type="int" value="19"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="20"/>
    <property name="stopGID" type="int" value="23"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="20"/>
    <property name="stopGID" type="int" value="26"/>
   </properties>
  </property>
  <property name="animation-height" type="int" value="1"/>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="stopGID" type="int" value="3"/>
   </properties>
  </property>
  <property name="animation-jump" type="class" propertytype="animation">
   <properties>
    <property name="isPermanent" type="bool" value="true"/>
    <property name="startGID" type="int" value="8"/>
    <property name="stopGID" type="int" value="15"/>
   </properties>
  </property>
  <property name="animation-update-rate" type="int" value="32"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="4"/>
    <property name="stopGID" type="int" value="7"/>
   </properties>
  </property>
  <property name="animation-width" type="int" value="1"/>
 </properties>
 <image source="../../graphics/sprites/entities/eg-slime/slime-full.png" width="384" height="320"/>
</tileset>
