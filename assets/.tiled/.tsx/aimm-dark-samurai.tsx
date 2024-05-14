<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="aimm-dark-samurai" tilewidth="64" tileheight="64" tilecount="112" columns="14">
 <properties>
  <property name="animation-attack-meele" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="28"/>
    <property name="stopGID" type="int" value="31"/>
   </properties>
  </property>
  <property name="animation-damaged" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="84"/>
    <property name="stopGID" type="int" value="85"/>
   </properties>
  </property>
  <property name="animation-death" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="98"/>
    <property name="stopGID" type="int" value="111"/>
    <property name="ticks-multiplier" type="float" value="2"/>
   </properties>
  </property>
  <property name="animation-idle" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="0"/>
    <property name="stopGID" type="int" value="7"/>
   </properties>
  </property>
  <property name="animation-ticks" type="int" value="100"/>
  <property name="animation-walk" type="class" propertytype="animation">
   <properties>
    <property name="startGID" type="int" value="14"/>
    <property name="stopGID" type="int" value="21"/>
   </properties>
  </property>
 </properties>
 <image source="../../graphics/sprites/entities/hostiles/aimm-dark-samurai.png" width="896" height="512"/>
</tileset>
