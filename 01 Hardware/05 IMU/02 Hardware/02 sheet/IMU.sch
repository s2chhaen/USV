<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.2">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="frames" urn="urn:adsk.eagle:library:229">
<description>&lt;b&gt;Frames for Sheet and Layout&lt;/b&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="A4L-LOC" urn="urn:adsk.eagle:symbol:13874/1" library_version="1">
<wire x1="256.54" y1="3.81" x2="256.54" y2="8.89" width="0.1016" layer="94"/>
<wire x1="256.54" y1="8.89" x2="256.54" y2="13.97" width="0.1016" layer="94"/>
<wire x1="256.54" y1="13.97" x2="256.54" y2="19.05" width="0.1016" layer="94"/>
<wire x1="256.54" y1="19.05" x2="256.54" y2="24.13" width="0.1016" layer="94"/>
<wire x1="161.29" y1="3.81" x2="161.29" y2="24.13" width="0.1016" layer="94"/>
<wire x1="161.29" y1="24.13" x2="215.265" y2="24.13" width="0.1016" layer="94"/>
<wire x1="215.265" y1="24.13" x2="256.54" y2="24.13" width="0.1016" layer="94"/>
<wire x1="246.38" y1="3.81" x2="246.38" y2="8.89" width="0.1016" layer="94"/>
<wire x1="246.38" y1="8.89" x2="256.54" y2="8.89" width="0.1016" layer="94"/>
<wire x1="246.38" y1="8.89" x2="215.265" y2="8.89" width="0.1016" layer="94"/>
<wire x1="215.265" y1="8.89" x2="215.265" y2="3.81" width="0.1016" layer="94"/>
<wire x1="215.265" y1="8.89" x2="215.265" y2="13.97" width="0.1016" layer="94"/>
<wire x1="215.265" y1="13.97" x2="256.54" y2="13.97" width="0.1016" layer="94"/>
<wire x1="215.265" y1="13.97" x2="215.265" y2="19.05" width="0.1016" layer="94"/>
<wire x1="215.265" y1="19.05" x2="256.54" y2="19.05" width="0.1016" layer="94"/>
<wire x1="215.265" y1="19.05" x2="215.265" y2="24.13" width="0.1016" layer="94"/>
<text x="217.17" y="15.24" size="2.54" layer="94">&gt;DRAWING_NAME</text>
<text x="217.17" y="10.16" size="2.286" layer="94">&gt;LAST_DATE_TIME</text>
<text x="230.505" y="5.08" size="2.54" layer="94">&gt;SHEET</text>
<text x="216.916" y="4.953" size="2.54" layer="94">Sheet:</text>
<frame x1="0" y1="0" x2="260.35" y2="179.07" columns="6" rows="4" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="A4L-LOC" urn="urn:adsk.eagle:component:13926/1" prefix="FRAME" uservalue="yes" library_version="1">
<description>&lt;b&gt;FRAME&lt;/b&gt;&lt;p&gt;
DIN A4, landscape with location and doc. field</description>
<gates>
<gate name="G$1" symbol="A4L-LOC" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="RpiPico">
<packages>
<package name="PICO-PKG">
<wire x1="10.5" y1="-25.5" x2="10.5" y2="-24.63" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-23.63" x2="10.5" y2="-22.09" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-21.09" x2="10.5" y2="-19.55" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-18.55" x2="10.5" y2="-17.01" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-16.01" x2="10.5" y2="-14.47" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-13.47" x2="10.5" y2="-11.93" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-10.93" x2="10.5" y2="-9.39" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-8.39" x2="10.5" y2="-6.85" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-5.85" x2="10.5" y2="-4.31" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-3.31" x2="10.5" y2="-1.77" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-0.77" x2="10.5" y2="0.77" width="0.1524" layer="21"/>
<wire x1="10.5" y1="1.77" x2="10.5" y2="3.31" width="0.1524" layer="21"/>
<wire x1="10.5" y1="4.31" x2="10.5" y2="5.85" width="0.1524" layer="21"/>
<wire x1="10.5" y1="6.85" x2="10.5" y2="8.39" width="0.1524" layer="21"/>
<wire x1="10.5" y1="9.39" x2="10.5" y2="10.93" width="0.1524" layer="21"/>
<wire x1="10.5" y1="11.93" x2="10.5" y2="13.47" width="0.1524" layer="21"/>
<wire x1="10.5" y1="14.47" x2="10.5" y2="16.01" width="0.1524" layer="21"/>
<wire x1="10.5" y1="17.01" x2="10.5" y2="18.55" width="0.1524" layer="21"/>
<wire x1="10.5" y1="19.55" x2="10.5" y2="21.09" width="0.1524" layer="21"/>
<wire x1="10.5" y1="22.09" x2="10.5" y2="23.63" width="0.1524" layer="21"/>
<wire x1="10.5" y1="24.63" x2="10.5" y2="25.5" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="25.5" x2="-10.5" y2="24.63" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="23.63" x2="-10.5" y2="22.09" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="21.09" x2="-10.5" y2="19.55" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="18.55" x2="-10.5" y2="17.01" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="16.01" x2="-10.5" y2="14.47" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="13.47" x2="-10.5" y2="11.93" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="10.93" x2="-10.5" y2="9.39" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="8.39" x2="-10.5" y2="6.85" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="5.85" x2="-10.5" y2="4.31" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="3.31" x2="-10.5" y2="1.77" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="0.77" x2="-10.5" y2="-0.77" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-1.77" x2="-10.5" y2="-3.31" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-4.31" x2="-10.5" y2="-5.85" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-6.85" x2="-10.5" y2="-8.39" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-9.39" x2="-10.5" y2="-10.93" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-11.93" x2="-10.5" y2="-13.47" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-14.47" x2="-10.5" y2="-16.01" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-17.01" x2="-10.5" y2="-18.55" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-19.55" x2="-10.5" y2="-21.09" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-22.09" x2="-10.5" y2="-23.63" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-24.63" x2="-10.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-25.5" x2="3.04" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="2.04" y1="-25.5" x2="0.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-0.5" y1="-25.5" x2="-2.04" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-3.04" y1="-25.5" x2="-10.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="25.5" x2="10.5" y2="25.5" width="0.1524" layer="21"/>
<pad name="1" x="-8.89" y="24.13" drill="1"/>
<pad name="2" x="-8.89" y="21.59" drill="1"/>
<pad name="7" x="-8.89" y="8.89" drill="1"/>
<pad name="8" x="-8.89" y="6.35" drill="1"/>
<pad name="3" x="-8.89" y="19.05" drill="1"/>
<pad name="4" x="-8.89" y="16.51" drill="1"/>
<pad name="6" x="-8.89" y="11.43" drill="1"/>
<pad name="5" x="-8.89" y="13.97" drill="1"/>
<pad name="9" x="-8.89" y="3.81" drill="1"/>
<pad name="10" x="-8.89" y="1.27" drill="1"/>
<pad name="11" x="-8.89" y="-1.27" drill="1"/>
<pad name="12" x="-8.89" y="-3.81" drill="1"/>
<pad name="13" x="-8.89" y="-6.35" drill="1"/>
<pad name="14" x="-8.89" y="-8.89" drill="1"/>
<pad name="15" x="-8.89" y="-11.43" drill="1"/>
<pad name="16" x="-8.89" y="-13.97" drill="1"/>
<pad name="17" x="-8.89" y="-16.51" drill="1"/>
<pad name="18" x="-8.89" y="-19.05" drill="1"/>
<pad name="19" x="-8.89" y="-21.59" drill="1"/>
<pad name="20" x="-8.89" y="-24.13" drill="1"/>
<pad name="21" x="8.89" y="-24.13" drill="1"/>
<pad name="22" x="8.89" y="-21.59" drill="1"/>
<pad name="23" x="8.89" y="-19.05" drill="1"/>
<pad name="24" x="8.89" y="-16.51" drill="1"/>
<pad name="25" x="8.89" y="-13.97" drill="1"/>
<pad name="26" x="8.89" y="-11.43" drill="1"/>
<pad name="27" x="8.89" y="-8.89" drill="1"/>
<pad name="28" x="8.89" y="-6.35" drill="1"/>
<pad name="29" x="8.89" y="-3.81" drill="1"/>
<pad name="30" x="8.89" y="-1.27" drill="1"/>
<pad name="31" x="8.89" y="1.27" drill="1"/>
<pad name="32" x="8.89" y="3.81" drill="1"/>
<pad name="33" x="8.89" y="6.35" drill="1"/>
<pad name="34" x="8.89" y="8.89" drill="1"/>
<pad name="35" x="8.89" y="11.43" drill="1"/>
<pad name="36" x="8.89" y="13.97" drill="1"/>
<pad name="37" x="8.89" y="16.51" drill="1"/>
<pad name="38" x="8.89" y="19.05" drill="1"/>
<pad name="39" x="8.89" y="21.59" drill="1"/>
<pad name="40" x="8.89" y="24.13" drill="1"/>
<text x="-10.414" y="-28.067" size="1.778" layer="25">&gt;NAME</text>
<text x="1.016" y="-20.955" size="1.778" layer="27" rot="R90">&gt;VALUE</text>
<smd name="P$1" x="-10" y="24.13" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$2" x="-10" y="21.59" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$3" x="-10" y="19.05" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$4" x="-10" y="16.51" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$5" x="-10" y="13.97" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$6" x="-10" y="11.43" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$7" x="-10" y="8.89" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$8" x="-10" y="6.35" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$9" x="-10" y="3.81" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$10" x="-10" y="1.27" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$11" x="-10" y="-1.27" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$12" x="-10" y="-3.81" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$13" x="-10" y="-6.35" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$14" x="-10" y="-8.89" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$15" x="-10" y="-11.43" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$16" x="-10" y="-13.97" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$17" x="-10" y="-16.51" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$18" x="-10" y="-19.05" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$19" x="-10" y="-21.59" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$20" x="-10" y="-24.13" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$21" x="10" y="-24.13" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$22" x="10" y="-21.59" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$23" x="10" y="-19.05" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$24" x="10" y="-16.51" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$25" x="10" y="-13.97" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$26" x="10" y="-11.43" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$27" x="10" y="-8.89" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$28" x="10" y="-6.35" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$29" x="10" y="-3.81" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$30" x="10" y="-1.27" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$31" x="10" y="1.27" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$32" x="10" y="3.81" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$33" x="10" y="6.35" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$34" x="10" y="8.89" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$35" x="10" y="11.43" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$36" x="10" y="13.97" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$37" x="10" y="16.51" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$38" x="10" y="19.05" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$39" x="10" y="21.59" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$40" x="10" y="24.13" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<hole x="-2.725" y="24" drill="1.8"/>
<hole x="2.725" y="24" drill="1.8"/>
<hole x="-2.425" y="20.97" drill="1.5"/>
<hole x="2.425" y="20.97" drill="1.5"/>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="18.5"/>
<vertex x="-1.5" y="18.5"/>
<vertex x="-1.5" y="16.5"/>
<vertex x="-3.5" y="16.5"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="16"/>
<vertex x="-1.5" y="16"/>
<vertex x="-1.5" y="14"/>
<vertex x="-3.5" y="14"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="13.5"/>
<vertex x="-1.5" y="13.5"/>
<vertex x="-1.5" y="11.5"/>
<vertex x="-3.5" y="11.5"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.7" y="24.9"/>
<vertex x="3.7" y="24.9"/>
<vertex x="3.7" y="20.2"/>
<vertex x="-3.7" y="20.2"/>
</polygon>
<pad name="41" x="-2.54" y="-23.9" drill="1"/>
<pad name="42" x="0" y="-23.9" drill="1"/>
<pad name="43" x="2.54" y="-23.9" drill="1"/>
<smd name="P$41" x="-2.54" y="-24.925" dx="3.5" dy="1.7" layer="1" rot="R270"/>
<smd name="P$42" x="0" y="-24.925" dx="3.5" dy="1.7" layer="1" rot="R270"/>
<smd name="P$43" x="2.54" y="-24.925" dx="3.5" dy="1.7" layer="1" rot="R270"/>
<circle x="-12.7" y="25.4" radius="0.635" width="0" layer="21"/>
<wire x1="-4" y1="26.8" x2="4" y2="26.8" width="0.127" layer="21"/>
<wire x1="4" y1="26.8" x2="4" y2="20" width="0.127" layer="21"/>
<wire x1="4" y1="20" x2="-4" y2="20" width="0.127" layer="21"/>
<wire x1="-4" y1="20" x2="-4" y2="26.8" width="0.127" layer="21"/>
<wire x1="-10.5" y1="22.09" x2="-10.5" y2="21.09" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="24.63" x2="-10.5" y2="23.63" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="19.55" x2="-10.5" y2="18.55" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="17.01" x2="-10.5" y2="16.01" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="14.47" x2="-10.5" y2="13.47" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="11.93" x2="-10.5" y2="10.93" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="9.39" x2="-10.5" y2="8.39" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="6.85" x2="-10.5" y2="5.85" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="4.31" x2="-10.5" y2="3.31" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="1.77" x2="-10.5" y2="0.77" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-0.77" x2="-10.5" y2="-1.77" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-3.31" x2="-10.5" y2="-4.31" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-5.85" x2="-10.5" y2="-6.85" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-8.39" x2="-10.5" y2="-9.39" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-10.93" x2="-10.5" y2="-11.93" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-13.47" x2="-10.5" y2="-14.47" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-16.01" x2="-10.5" y2="-17.01" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-18.55" x2="-10.5" y2="-19.55" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-21.09" x2="-10.5" y2="-22.09" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-23.63" x2="-10.5" y2="-24.63" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-24.63" x2="10.5" y2="-23.63" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-22.09" x2="10.5" y2="-21.09" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-19.55" x2="10.5" y2="-18.55" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-17.01" x2="10.5" y2="-16.01" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-14.47" x2="10.5" y2="-13.47" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-11.93" x2="10.5" y2="-10.93" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-9.39" x2="10.5" y2="-8.39" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-6.85" x2="10.5" y2="-5.85" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-4.31" x2="10.5" y2="-3.31" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-1.77" x2="10.5" y2="-0.77" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="0.77" x2="10.5" y2="1.77" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="3.31" x2="10.5" y2="4.31" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="5.85" x2="10.5" y2="6.85" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="8.39" x2="10.5" y2="9.39" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="10.93" x2="10.5" y2="11.93" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="13.47" x2="10.5" y2="14.47" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="16.01" x2="10.5" y2="17.01" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="18.55" x2="10.5" y2="19.55" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="21.09" x2="10.5" y2="22.09" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="23.63" x2="10.5" y2="24.63" width="0.127" layer="21" curve="-180"/>
<wire x1="-0.5" y1="-25.5" x2="0.5" y2="-25.5" width="0.127" layer="21" curve="-180"/>
<wire x1="2.04" y1="-25.5" x2="3.04" y2="-25.5" width="0.127" layer="21" curve="-180"/>
<wire x1="-3.04" y1="-25.5" x2="-2.04" y2="-25.5" width="0.127" layer="21" curve="-180"/>
</package>
<package name="PICO-PKG-SMD">
<wire x1="-10.5" y1="25.5" x2="10.5" y2="25.5" width="0.1524" layer="21"/>
<text x="-10.414" y="-28.067" size="1.778" layer="25">&gt;NAME</text>
<text x="1.016" y="-20.955" size="1.778" layer="27" rot="R90">&gt;VALUE</text>
<smd name="P$1" x="-10" y="24.13" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$2" x="-10" y="21.59" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$3" x="-10" y="19.05" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$4" x="-10" y="16.51" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$5" x="-10" y="13.97" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$6" x="-10" y="11.43" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$7" x="-10" y="8.89" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$8" x="-10" y="6.35" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$9" x="-10" y="3.81" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$10" x="-10" y="1.27" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$11" x="-10" y="-1.27" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$12" x="-10" y="-3.81" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$13" x="-10" y="-6.35" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$14" x="-10" y="-8.89" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$15" x="-10" y="-11.43" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$16" x="-10" y="-13.97" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$17" x="-10" y="-16.51" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$18" x="-10" y="-19.05" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$19" x="-10" y="-21.59" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$20" x="-10" y="-24.13" dx="3.5" dy="1.7" layer="1"/>
<smd name="P$21" x="10" y="-24.13" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$22" x="10" y="-21.59" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$23" x="10" y="-19.05" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$24" x="10" y="-16.51" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$25" x="10" y="-13.97" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$26" x="10" y="-11.43" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$27" x="10" y="-8.89" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$28" x="10" y="-6.35" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$29" x="10" y="-3.81" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$30" x="10" y="-1.27" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$31" x="10" y="1.27" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$32" x="10" y="3.81" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$33" x="10" y="6.35" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$34" x="10" y="8.89" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$35" x="10" y="11.43" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$36" x="10" y="13.97" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$37" x="10" y="16.51" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$38" x="10" y="19.05" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$39" x="10" y="21.59" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<smd name="P$40" x="10" y="24.13" dx="3.5" dy="1.7" layer="1" rot="R180"/>
<hole x="-2.725" y="24" drill="1.8"/>
<hole x="2.725" y="24" drill="1.8"/>
<hole x="-2.425" y="20.97" drill="1.5"/>
<hole x="2.425" y="20.97" drill="1.5"/>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="18.5"/>
<vertex x="-1.5" y="18.5"/>
<vertex x="-1.5" y="16.5"/>
<vertex x="-3.5" y="16.5"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="16"/>
<vertex x="-1.5" y="16"/>
<vertex x="-1.5" y="14"/>
<vertex x="-3.5" y="14"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="13.5"/>
<vertex x="-1.5" y="13.5"/>
<vertex x="-1.5" y="11.5"/>
<vertex x="-3.5" y="11.5"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.7" y="24.9"/>
<vertex x="3.7" y="24.9"/>
<vertex x="3.7" y="20.2"/>
<vertex x="-3.7" y="20.2"/>
</polygon>
<smd name="P$41" x="-2.54" y="-24.925" dx="3.5" dy="1.7" layer="1" rot="R270"/>
<smd name="P$42" x="0" y="-24.925" dx="3.5" dy="1.7" layer="1" rot="R270"/>
<smd name="P$43" x="2.54" y="-24.925" dx="3.5" dy="1.7" layer="1" rot="R270"/>
<circle x="-12.7" y="25.4" radius="0.635" width="0" layer="21"/>
<wire x1="-4" y1="26.8" x2="4" y2="26.8" width="0.127" layer="21"/>
<wire x1="4" y1="26.8" x2="4" y2="20" width="0.127" layer="21"/>
<wire x1="4" y1="20" x2="-4" y2="20" width="0.127" layer="21"/>
<wire x1="-4" y1="20" x2="-4" y2="26.8" width="0.127" layer="21"/>
<wire x1="10.5" y1="-25.5" x2="10.5" y2="-24.63" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-23.63" x2="10.5" y2="-22.09" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-21.09" x2="10.5" y2="-19.55" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-18.55" x2="10.5" y2="-17.01" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-16.01" x2="10.5" y2="-14.47" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-13.47" x2="10.5" y2="-11.93" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-10.93" x2="10.5" y2="-9.39" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-8.39" x2="10.5" y2="-6.85" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-5.85" x2="10.5" y2="-4.31" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-3.31" x2="10.5" y2="-1.77" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-0.77" x2="10.5" y2="0.77" width="0.1524" layer="21"/>
<wire x1="10.5" y1="1.77" x2="10.5" y2="3.31" width="0.1524" layer="21"/>
<wire x1="10.5" y1="4.31" x2="10.5" y2="5.85" width="0.1524" layer="21"/>
<wire x1="10.5" y1="6.85" x2="10.5" y2="8.39" width="0.1524" layer="21"/>
<wire x1="10.5" y1="9.39" x2="10.5" y2="10.93" width="0.1524" layer="21"/>
<wire x1="10.5" y1="11.93" x2="10.5" y2="13.47" width="0.1524" layer="21"/>
<wire x1="10.5" y1="14.47" x2="10.5" y2="16.01" width="0.1524" layer="21"/>
<wire x1="10.5" y1="17.01" x2="10.5" y2="18.55" width="0.1524" layer="21"/>
<wire x1="10.5" y1="19.55" x2="10.5" y2="21.09" width="0.1524" layer="21"/>
<wire x1="10.5" y1="22.09" x2="10.5" y2="23.63" width="0.1524" layer="21"/>
<wire x1="10.5" y1="24.63" x2="10.5" y2="25.5" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="25.5" x2="-10.5" y2="24.63" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="23.63" x2="-10.5" y2="22.09" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="21.09" x2="-10.5" y2="19.55" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="18.55" x2="-10.5" y2="17.01" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="16.01" x2="-10.5" y2="14.47" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="13.47" x2="-10.5" y2="11.93" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="10.93" x2="-10.5" y2="9.39" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="8.39" x2="-10.5" y2="6.85" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="5.85" x2="-10.5" y2="4.31" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="3.31" x2="-10.5" y2="1.77" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="0.77" x2="-10.5" y2="-0.77" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-1.77" x2="-10.5" y2="-3.31" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-4.31" x2="-10.5" y2="-5.85" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-6.85" x2="-10.5" y2="-8.39" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-9.39" x2="-10.5" y2="-10.93" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-11.93" x2="-10.5" y2="-13.47" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-14.47" x2="-10.5" y2="-16.01" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-17.01" x2="-10.5" y2="-18.55" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-19.55" x2="-10.5" y2="-21.09" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-22.09" x2="-10.5" y2="-23.63" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-24.63" x2="-10.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-25.5" x2="3.04" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="2.04" y1="-25.5" x2="0.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-0.5" y1="-25.5" x2="-2.04" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-3.04" y1="-25.5" x2="-10.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="22.09" x2="-10.5" y2="21.09" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="24.63" x2="-10.5" y2="23.63" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="19.55" x2="-10.5" y2="18.55" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="17.01" x2="-10.5" y2="16.01" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="14.47" x2="-10.5" y2="13.47" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="11.93" x2="-10.5" y2="10.93" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="9.39" x2="-10.5" y2="8.39" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="6.85" x2="-10.5" y2="5.85" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="4.31" x2="-10.5" y2="3.31" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="1.77" x2="-10.5" y2="0.77" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-0.77" x2="-10.5" y2="-1.77" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-3.31" x2="-10.5" y2="-4.31" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-5.85" x2="-10.5" y2="-6.85" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-8.39" x2="-10.5" y2="-9.39" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-10.93" x2="-10.5" y2="-11.93" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-13.47" x2="-10.5" y2="-14.47" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-16.01" x2="-10.5" y2="-17.01" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-18.55" x2="-10.5" y2="-19.55" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-21.09" x2="-10.5" y2="-22.09" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-23.63" x2="-10.5" y2="-24.63" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-24.63" x2="10.5" y2="-23.63" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-22.09" x2="10.5" y2="-21.09" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-19.55" x2="10.5" y2="-18.55" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-17.01" x2="10.5" y2="-16.01" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-14.47" x2="10.5" y2="-13.47" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-11.93" x2="10.5" y2="-10.93" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-9.39" x2="10.5" y2="-8.39" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-6.85" x2="10.5" y2="-5.85" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-4.31" x2="10.5" y2="-3.31" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-1.77" x2="10.5" y2="-0.77" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="0.77" x2="10.5" y2="1.77" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="3.31" x2="10.5" y2="4.31" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="5.85" x2="10.5" y2="6.85" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="8.39" x2="10.5" y2="9.39" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="10.93" x2="10.5" y2="11.93" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="13.47" x2="10.5" y2="14.47" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="16.01" x2="10.5" y2="17.01" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="18.55" x2="10.5" y2="19.55" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="21.09" x2="10.5" y2="22.09" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="23.63" x2="10.5" y2="24.63" width="0.127" layer="21" curve="-180"/>
<wire x1="-0.5" y1="-25.5" x2="0.5" y2="-25.5" width="0.127" layer="21" curve="-180"/>
<wire x1="2.04" y1="-25.5" x2="3.04" y2="-25.5" width="0.127" layer="21" curve="-180"/>
<wire x1="-3.04" y1="-25.5" x2="-2.04" y2="-25.5" width="0.127" layer="21" curve="-180"/>
</package>
<package name="PICO-PKG-TH">
<wire x1="-10.5" y1="25.5" x2="10.5" y2="25.5" width="0.1524" layer="21"/>
<pad name="1" x="-8.89" y="24.13" drill="1"/>
<pad name="2" x="-8.89" y="21.59" drill="1"/>
<pad name="7" x="-8.89" y="8.89" drill="1"/>
<pad name="8" x="-8.89" y="6.35" drill="1"/>
<pad name="3" x="-8.89" y="19.05" drill="1"/>
<pad name="4" x="-8.89" y="16.51" drill="1"/>
<pad name="6" x="-8.89" y="11.43" drill="1"/>
<pad name="5" x="-8.89" y="13.97" drill="1"/>
<pad name="9" x="-8.89" y="3.81" drill="1"/>
<pad name="10" x="-8.89" y="1.27" drill="1"/>
<pad name="11" x="-8.89" y="-1.27" drill="1"/>
<pad name="12" x="-8.89" y="-3.81" drill="1"/>
<pad name="13" x="-8.89" y="-6.35" drill="1"/>
<pad name="14" x="-8.89" y="-8.89" drill="1"/>
<pad name="15" x="-8.89" y="-11.43" drill="1"/>
<pad name="16" x="-8.89" y="-13.97" drill="1"/>
<pad name="17" x="-8.89" y="-16.51" drill="1"/>
<pad name="18" x="-8.89" y="-19.05" drill="1"/>
<pad name="19" x="-8.89" y="-21.59" drill="1"/>
<pad name="20" x="-8.89" y="-24.13" drill="1"/>
<pad name="21" x="8.89" y="-24.13" drill="1"/>
<pad name="22" x="8.89" y="-21.59" drill="1"/>
<pad name="23" x="8.89" y="-19.05" drill="1"/>
<pad name="24" x="8.89" y="-16.51" drill="1"/>
<pad name="25" x="8.89" y="-13.97" drill="1"/>
<pad name="26" x="8.89" y="-11.43" drill="1"/>
<pad name="27" x="8.89" y="-8.89" drill="1"/>
<pad name="28" x="8.89" y="-6.35" drill="1"/>
<pad name="29" x="8.89" y="-3.81" drill="1"/>
<pad name="30" x="8.89" y="-1.27" drill="1"/>
<pad name="31" x="8.89" y="1.27" drill="1"/>
<pad name="32" x="8.89" y="3.81" drill="1"/>
<pad name="33" x="8.89" y="6.35" drill="1"/>
<pad name="34" x="8.89" y="8.89" drill="1"/>
<pad name="35" x="8.89" y="11.43" drill="1"/>
<pad name="36" x="8.89" y="13.97" drill="1"/>
<pad name="37" x="8.89" y="16.51" drill="1"/>
<pad name="38" x="8.89" y="19.05" drill="1"/>
<pad name="39" x="8.89" y="21.59" drill="1"/>
<pad name="40" x="8.89" y="24.13" drill="1"/>
<text x="-10.414" y="-28.067" size="1.778" layer="25">&gt;NAME</text>
<text x="1.016" y="-20.955" size="1.778" layer="27" rot="R90">&gt;VALUE</text>
<hole x="-2.725" y="24" drill="1.8"/>
<hole x="2.725" y="24" drill="1.8"/>
<hole x="-2.425" y="20.97" drill="1.5"/>
<hole x="2.425" y="20.97" drill="1.5"/>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="18.5"/>
<vertex x="-1.5" y="18.5"/>
<vertex x="-1.5" y="16.5"/>
<vertex x="-3.5" y="16.5"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="16"/>
<vertex x="-1.5" y="16"/>
<vertex x="-1.5" y="14"/>
<vertex x="-3.5" y="14"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.5" y="13.5"/>
<vertex x="-1.5" y="13.5"/>
<vertex x="-1.5" y="11.5"/>
<vertex x="-3.5" y="11.5"/>
</polygon>
<polygon width="0.127" layer="41">
<vertex x="-3.7" y="24.9"/>
<vertex x="3.7" y="24.9"/>
<vertex x="3.7" y="20.2"/>
<vertex x="-3.7" y="20.2"/>
</polygon>
<pad name="41" x="-2.54" y="-23.9" drill="1"/>
<pad name="42" x="0" y="-23.9" drill="1"/>
<pad name="43" x="2.54" y="-23.9" drill="1"/>
<circle x="-12.7" y="25.4" radius="0.635" width="0" layer="21"/>
<wire x1="-4" y1="26.8" x2="4" y2="26.8" width="0.127" layer="21"/>
<wire x1="4" y1="26.8" x2="4" y2="20" width="0.127" layer="21"/>
<wire x1="4" y1="20" x2="-4" y2="20" width="0.127" layer="21"/>
<wire x1="-4" y1="20" x2="-4" y2="26.8" width="0.127" layer="21"/>
<wire x1="10.5" y1="-25.5" x2="10.5" y2="-24.63" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-23.63" x2="10.5" y2="-22.09" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-21.09" x2="10.5" y2="-19.55" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-18.55" x2="10.5" y2="-17.01" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-16.01" x2="10.5" y2="-14.47" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-13.47" x2="10.5" y2="-11.93" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-10.93" x2="10.5" y2="-9.39" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-8.39" x2="10.5" y2="-6.85" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-5.85" x2="10.5" y2="-4.31" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-3.31" x2="10.5" y2="-1.77" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-0.77" x2="10.5" y2="0.77" width="0.1524" layer="21"/>
<wire x1="10.5" y1="1.77" x2="10.5" y2="3.31" width="0.1524" layer="21"/>
<wire x1="10.5" y1="4.31" x2="10.5" y2="5.85" width="0.1524" layer="21"/>
<wire x1="10.5" y1="6.85" x2="10.5" y2="8.39" width="0.1524" layer="21"/>
<wire x1="10.5" y1="9.39" x2="10.5" y2="10.93" width="0.1524" layer="21"/>
<wire x1="10.5" y1="11.93" x2="10.5" y2="13.47" width="0.1524" layer="21"/>
<wire x1="10.5" y1="14.47" x2="10.5" y2="16.01" width="0.1524" layer="21"/>
<wire x1="10.5" y1="17.01" x2="10.5" y2="18.55" width="0.1524" layer="21"/>
<wire x1="10.5" y1="19.55" x2="10.5" y2="21.09" width="0.1524" layer="21"/>
<wire x1="10.5" y1="22.09" x2="10.5" y2="23.63" width="0.1524" layer="21"/>
<wire x1="10.5" y1="24.63" x2="10.5" y2="25.5" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="25.5" x2="-10.5" y2="24.63" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="23.63" x2="-10.5" y2="22.09" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="21.09" x2="-10.5" y2="19.55" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="18.55" x2="-10.5" y2="17.01" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="16.01" x2="-10.5" y2="14.47" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="13.47" x2="-10.5" y2="11.93" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="10.93" x2="-10.5" y2="9.39" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="8.39" x2="-10.5" y2="6.85" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="5.85" x2="-10.5" y2="4.31" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="3.31" x2="-10.5" y2="1.77" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="0.77" x2="-10.5" y2="-0.77" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-1.77" x2="-10.5" y2="-3.31" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-4.31" x2="-10.5" y2="-5.85" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-6.85" x2="-10.5" y2="-8.39" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-9.39" x2="-10.5" y2="-10.93" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-11.93" x2="-10.5" y2="-13.47" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-14.47" x2="-10.5" y2="-16.01" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-17.01" x2="-10.5" y2="-18.55" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-19.55" x2="-10.5" y2="-21.09" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-22.09" x2="-10.5" y2="-23.63" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="-24.63" x2="-10.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="10.5" y1="-25.5" x2="3.04" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="2.04" y1="-25.5" x2="0.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-0.5" y1="-25.5" x2="-2.04" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-3.04" y1="-25.5" x2="-10.5" y2="-25.5" width="0.1524" layer="21"/>
<wire x1="-10.5" y1="22.09" x2="-10.5" y2="21.09" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="24.63" x2="-10.5" y2="23.63" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="19.55" x2="-10.5" y2="18.55" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="17.01" x2="-10.5" y2="16.01" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="14.47" x2="-10.5" y2="13.47" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="11.93" x2="-10.5" y2="10.93" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="9.39" x2="-10.5" y2="8.39" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="6.85" x2="-10.5" y2="5.85" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="4.31" x2="-10.5" y2="3.31" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="1.77" x2="-10.5" y2="0.77" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-0.77" x2="-10.5" y2="-1.77" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-3.31" x2="-10.5" y2="-4.31" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-5.85" x2="-10.5" y2="-6.85" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-8.39" x2="-10.5" y2="-9.39" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-10.93" x2="-10.5" y2="-11.93" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-13.47" x2="-10.5" y2="-14.47" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-16.01" x2="-10.5" y2="-17.01" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-18.55" x2="-10.5" y2="-19.55" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-21.09" x2="-10.5" y2="-22.09" width="0.127" layer="21" curve="-180"/>
<wire x1="-10.5" y1="-23.63" x2="-10.5" y2="-24.63" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-24.63" x2="10.5" y2="-23.63" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-22.09" x2="10.5" y2="-21.09" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-19.55" x2="10.5" y2="-18.55" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-17.01" x2="10.5" y2="-16.01" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-14.47" x2="10.5" y2="-13.47" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-11.93" x2="10.5" y2="-10.93" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-9.39" x2="10.5" y2="-8.39" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-6.85" x2="10.5" y2="-5.85" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-4.31" x2="10.5" y2="-3.31" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="-1.77" x2="10.5" y2="-0.77" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="0.77" x2="10.5" y2="1.77" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="3.31" x2="10.5" y2="4.31" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="5.85" x2="10.5" y2="6.85" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="8.39" x2="10.5" y2="9.39" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="10.93" x2="10.5" y2="11.93" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="13.47" x2="10.5" y2="14.47" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="16.01" x2="10.5" y2="17.01" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="18.55" x2="10.5" y2="19.55" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="21.09" x2="10.5" y2="22.09" width="0.127" layer="21" curve="-180"/>
<wire x1="10.5" y1="23.63" x2="10.5" y2="24.63" width="0.127" layer="21" curve="-180"/>
<wire x1="-0.5" y1="-25.5" x2="0.5" y2="-25.5" width="0.127" layer="21" curve="-180"/>
<wire x1="2.04" y1="-25.5" x2="3.04" y2="-25.5" width="0.127" layer="21" curve="-180"/>
<wire x1="-3.04" y1="-25.5" x2="-2.04" y2="-25.5" width="0.127" layer="21" curve="-180"/>
</package>
</packages>
<symbols>
<symbol name="PICO-SYM">
<pin name="GP0" x="-17.78" y="25.4" length="middle"/>
<pin name="GP1" x="-17.78" y="22.86" length="middle"/>
<pin name="GP2" x="-17.78" y="17.78" length="middle"/>
<pin name="GP3" x="-17.78" y="15.24" length="middle"/>
<pin name="GP4" x="-17.78" y="12.7" length="middle"/>
<pin name="GP5" x="-17.78" y="10.16" length="middle"/>
<pin name="GP6" x="-17.78" y="5.08" length="middle"/>
<pin name="GP7" x="-17.78" y="2.54" length="middle"/>
<pin name="GP8" x="-17.78" y="0" length="middle"/>
<pin name="GP9" x="-17.78" y="-2.54" length="middle"/>
<pin name="GP10" x="-17.78" y="-7.62" length="middle"/>
<pin name="GP11" x="-17.78" y="-10.16" length="middle"/>
<pin name="GP12" x="-17.78" y="-12.7" length="middle"/>
<pin name="GP13" x="-17.78" y="-15.24" length="middle"/>
<pin name="GP14" x="-17.78" y="-20.32" length="middle"/>
<pin name="GP15" x="-17.78" y="-22.86" length="middle"/>
<pin name="GP16" x="17.78" y="-22.86" length="middle" rot="R180"/>
<pin name="GP17" x="17.78" y="-20.32" length="middle" rot="R180"/>
<pin name="GP18" x="17.78" y="-15.24" length="middle" rot="R180"/>
<pin name="GP19" x="17.78" y="-12.7" length="middle" rot="R180"/>
<pin name="GP20" x="17.78" y="-10.16" length="middle" rot="R180"/>
<pin name="GP21" x="17.78" y="-7.62" length="middle" rot="R180"/>
<pin name="GP22" x="17.78" y="-2.54" length="middle" rot="R180"/>
<pin name="RUN" x="17.78" y="22.86" length="middle" direction="in" rot="R180"/>
<pin name="GP26" x="17.78" y="2.54" length="middle" rot="R180"/>
<pin name="GP27" x="17.78" y="5.08" length="middle" rot="R180"/>
<pin name="GP28" x="17.78" y="10.16" length="middle" rot="R180"/>
<pin name="ADC_REF" x="17.78" y="17.78" length="middle" direction="in" rot="R180"/>
<pin name="3V3(OUT)" x="17.78" y="30.48" length="middle" direction="pwr" rot="R180"/>
<pin name="3V3_EN" x="17.78" y="25.4" length="middle" direction="in" rot="R180"/>
<pin name="GND" x="17.78" y="-35.56" length="middle" direction="pwr" rot="R180"/>
<pin name="VSYS" x="17.78" y="33.02" length="middle" direction="pwr" rot="R180"/>
<pin name="VBUS" x="17.78" y="35.56" length="middle" direction="pwr" rot="R180"/>
<wire x1="-12.7" y1="38.1" x2="12.7" y2="38.1" width="0.254" layer="94"/>
<wire x1="12.7" y1="38.1" x2="12.7" y2="-38.1" width="0.254" layer="94"/>
<wire x1="12.7" y1="-38.1" x2="-12.7" y2="-38.1" width="0.254" layer="94"/>
<wire x1="-12.7" y1="-38.1" x2="-12.7" y2="38.1" width="0.254" layer="94"/>
<text x="-12.7155" y="39.4146" size="2.54388125" layer="95">&gt;NAME</text>
<text x="-12.7069" y="-41.9314" size="2.54171875" layer="96">&gt;VALUE</text>
<pin name="SWDIO" x="-17.78" y="-35.56" length="middle"/>
<pin name="SWGND" x="-17.78" y="-33.02" length="middle" direction="pwr"/>
<pin name="SWCLK" x="-17.78" y="-30.48" length="middle" direction="in"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="RASPBERRY_PICO" prefix="U">
<description>Raspberry Pi Pico</description>
<gates>
<gate name="U$1" symbol="PICO-SYM" x="0" y="-20.32"/>
</gates>
<devices>
<device name="SMD-TH" package="PICO-PKG">
<connects>
<connect gate="U$1" pin="3V3(OUT)" pad="36 P$36"/>
<connect gate="U$1" pin="3V3_EN" pad="37 P$37"/>
<connect gate="U$1" pin="ADC_REF" pad="35 P$35"/>
<connect gate="U$1" pin="GND" pad="3 8 13 18 23 28 33 38 P$3 P$8 P$13 P$18 P$23 P$28 P$33 P$38"/>
<connect gate="U$1" pin="GP0" pad="1 P$1"/>
<connect gate="U$1" pin="GP1" pad="2 P$2"/>
<connect gate="U$1" pin="GP10" pad="14 P$14"/>
<connect gate="U$1" pin="GP11" pad="15 P$15"/>
<connect gate="U$1" pin="GP12" pad="16 P$16"/>
<connect gate="U$1" pin="GP13" pad="17 P$17"/>
<connect gate="U$1" pin="GP14" pad="19 P$19"/>
<connect gate="U$1" pin="GP15" pad="20 P$20"/>
<connect gate="U$1" pin="GP16" pad="21 P$21"/>
<connect gate="U$1" pin="GP17" pad="22 P$22"/>
<connect gate="U$1" pin="GP18" pad="24 P$24"/>
<connect gate="U$1" pin="GP19" pad="25 P$25"/>
<connect gate="U$1" pin="GP2" pad="4 P$4"/>
<connect gate="U$1" pin="GP20" pad="26 P$26"/>
<connect gate="U$1" pin="GP21" pad="27 P$27"/>
<connect gate="U$1" pin="GP22" pad="29 P$29"/>
<connect gate="U$1" pin="GP26" pad="31 P$31"/>
<connect gate="U$1" pin="GP27" pad="32 P$32"/>
<connect gate="U$1" pin="GP28" pad="34 P$34"/>
<connect gate="U$1" pin="GP3" pad="5 P$5"/>
<connect gate="U$1" pin="GP4" pad="6 P$6"/>
<connect gate="U$1" pin="GP5" pad="7 P$7"/>
<connect gate="U$1" pin="GP6" pad="9 P$9"/>
<connect gate="U$1" pin="GP7" pad="10 P$10"/>
<connect gate="U$1" pin="GP8" pad="11 P$11"/>
<connect gate="U$1" pin="GP9" pad="12 P$12"/>
<connect gate="U$1" pin="RUN" pad="30 P$30"/>
<connect gate="U$1" pin="SWCLK" pad="41 P$41"/>
<connect gate="U$1" pin="SWDIO" pad="43 P$43"/>
<connect gate="U$1" pin="SWGND" pad="42 P$42"/>
<connect gate="U$1" pin="VBUS" pad="40 P$40"/>
<connect gate="U$1" pin="VSYS" pad="39 P$39"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMD" package="PICO-PKG-SMD">
<connects>
<connect gate="U$1" pin="3V3(OUT)" pad="P$36"/>
<connect gate="U$1" pin="3V3_EN" pad="P$37"/>
<connect gate="U$1" pin="ADC_REF" pad="P$35"/>
<connect gate="U$1" pin="GND" pad="P$3 P$8 P$13 P$18 P$23 P$28 P$33 P$38"/>
<connect gate="U$1" pin="GP0" pad="P$1"/>
<connect gate="U$1" pin="GP1" pad="P$2"/>
<connect gate="U$1" pin="GP10" pad="P$14"/>
<connect gate="U$1" pin="GP11" pad="P$15"/>
<connect gate="U$1" pin="GP12" pad="P$16"/>
<connect gate="U$1" pin="GP13" pad="P$17"/>
<connect gate="U$1" pin="GP14" pad="P$19"/>
<connect gate="U$1" pin="GP15" pad="P$20"/>
<connect gate="U$1" pin="GP16" pad="P$21"/>
<connect gate="U$1" pin="GP17" pad="P$22"/>
<connect gate="U$1" pin="GP18" pad="P$24"/>
<connect gate="U$1" pin="GP19" pad="P$25"/>
<connect gate="U$1" pin="GP2" pad="P$4"/>
<connect gate="U$1" pin="GP20" pad="P$26"/>
<connect gate="U$1" pin="GP21" pad="P$27"/>
<connect gate="U$1" pin="GP22" pad="P$29"/>
<connect gate="U$1" pin="GP26" pad="P$31"/>
<connect gate="U$1" pin="GP27" pad="P$32"/>
<connect gate="U$1" pin="GP28" pad="P$34"/>
<connect gate="U$1" pin="GP3" pad="P$5"/>
<connect gate="U$1" pin="GP4" pad="P$6"/>
<connect gate="U$1" pin="GP5" pad="P$7"/>
<connect gate="U$1" pin="GP6" pad="P$9"/>
<connect gate="U$1" pin="GP7" pad="P$10"/>
<connect gate="U$1" pin="GP8" pad="P$11"/>
<connect gate="U$1" pin="GP9" pad="P$12"/>
<connect gate="U$1" pin="RUN" pad="P$30"/>
<connect gate="U$1" pin="SWCLK" pad="P$41"/>
<connect gate="U$1" pin="SWDIO" pad="P$43"/>
<connect gate="U$1" pin="SWGND" pad="P$42"/>
<connect gate="U$1" pin="VBUS" pad="P$40"/>
<connect gate="U$1" pin="VSYS" pad="P$39"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="TH" package="PICO-PKG-TH">
<connects>
<connect gate="U$1" pin="3V3(OUT)" pad="36"/>
<connect gate="U$1" pin="3V3_EN" pad="37"/>
<connect gate="U$1" pin="ADC_REF" pad="35"/>
<connect gate="U$1" pin="GND" pad="3 8 13 18 23 28 33 38"/>
<connect gate="U$1" pin="GP0" pad="1"/>
<connect gate="U$1" pin="GP1" pad="2"/>
<connect gate="U$1" pin="GP10" pad="14"/>
<connect gate="U$1" pin="GP11" pad="15"/>
<connect gate="U$1" pin="GP12" pad="16"/>
<connect gate="U$1" pin="GP13" pad="17"/>
<connect gate="U$1" pin="GP14" pad="19"/>
<connect gate="U$1" pin="GP15" pad="20"/>
<connect gate="U$1" pin="GP16" pad="21"/>
<connect gate="U$1" pin="GP17" pad="22"/>
<connect gate="U$1" pin="GP18" pad="24"/>
<connect gate="U$1" pin="GP19" pad="25"/>
<connect gate="U$1" pin="GP2" pad="4"/>
<connect gate="U$1" pin="GP20" pad="26"/>
<connect gate="U$1" pin="GP21" pad="27"/>
<connect gate="U$1" pin="GP22" pad="29"/>
<connect gate="U$1" pin="GP26" pad="31"/>
<connect gate="U$1" pin="GP27" pad="32"/>
<connect gate="U$1" pin="GP28" pad="34"/>
<connect gate="U$1" pin="GP3" pad="5"/>
<connect gate="U$1" pin="GP4" pad="6"/>
<connect gate="U$1" pin="GP5" pad="7"/>
<connect gate="U$1" pin="GP6" pad="9"/>
<connect gate="U$1" pin="GP7" pad="10"/>
<connect gate="U$1" pin="GP8" pad="11"/>
<connect gate="U$1" pin="GP9" pad="12"/>
<connect gate="U$1" pin="RUN" pad="30"/>
<connect gate="U$1" pin="SWCLK" pad="41"/>
<connect gate="U$1" pin="SWDIO" pad="43"/>
<connect gate="U$1" pin="SWGND" pad="42"/>
<connect gate="U$1" pin="VBUS" pad="40"/>
<connect gate="U$1" pin="VSYS" pad="39"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="FRAME1" library="frames" library_urn="urn:adsk.eagle:library:229" deviceset="A4L-LOC" device=""/>
<part name="U2" library="RpiPico" deviceset="RASPBERRY_PICO" device="TH"/>
</parts>
<sheets>
<sheet>
<plain>
<text x="180.34" y="10.16" size="2.54" layer="97">IMU - Unit</text>
<text x="177.8" y="15.24" size="2.54" layer="97">USV - Projekt</text>
<text x="248.92" y="5.08" size="1.778" layer="97">V 1.0</text>
</plain>
<instances>
<instance part="FRAME1" gate="G$1" x="0" y="0" smashed="yes">
<attribute name="DRAWING_NAME" x="217.17" y="15.24" size="2.54" layer="94"/>
<attribute name="LAST_DATE_TIME" x="217.17" y="10.16" size="2.286" layer="94"/>
<attribute name="SHEET" x="230.505" y="5.08" size="2.54" layer="94"/>
</instance>
<instance part="U2" gate="U$1" x="53.34" y="116.84" smashed="yes">
<attribute name="NAME" x="40.6245" y="156.2546" size="2.54388125" layer="95"/>
<attribute name="VALUE" x="40.6331" y="74.9086" size="2.54171875" layer="96"/>
</instance>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
