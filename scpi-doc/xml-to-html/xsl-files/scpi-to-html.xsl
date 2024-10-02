<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="templates/strings.xsl"/>

<!-- Template names follow file names -->
<xsl:include href="templates/zera-logo.xsl"/>
<xsl:include href="templates/header-global.xsl"/>
<xsl:include href="templates/current-date.xsl"/>
<xsl:include href="templates/device-info.xsl"/>
<xsl:include href="templates/system-commands.xsl"/>
<xsl:include href="templates/error-calculators.xsl"/>
<xsl:include href="templates/measurement.xsl"/>
<xsl:include href="templates/sense-range.xsl"/>
<xsl:include href="templates/system-info.xsl"/>
<xsl:include href="templates/sampling-configs.xsl"/>
<xsl:include href="templates/scpi-standard-cmds.xsl"/>
<xsl:include href="templates/dev-iface.xsl"/>
<xsl:include href="templates/pow-meas-prosa.xsl"/>
<xsl:include href="templates/chapter-number.xsl"/>
<xsl:include href="templates/adjustment.xsl"/>

<xsl:param name="zenuxVersion"/>
<xsl:param name="sessionXml"/>
<xsl:param name="adjustmentOutput"/>

<xsl:template match="MODELLIST">
  <!-- Sequencce as in document -->
  <html> 
  <title>
    <xsl:call-template name="HeaderGlobal">
      <xsl:with-param name="adjustmentData" select="$adjustmentOutput"/>
    </xsl:call-template>
  </title>

  <head>
    <style>
      :root {
      /*default: light-theme*/
          --color-bg: #fbf8f8;
          --color-fg: black;
          --color-table-tr: #dddddd;
          --color-table-tr-btm: #f3f3f3;
          --color-table-border: white;
          --color-table-border-btm: #206040;
          --color-table-border-thickness: 2px;
      }
      .dark-theme {
          --color-bg: #484343;
          --color-fg: white;
          --color-table-tr: #666565;
          --color-table-tr-btm: #666565;
          --color-table-border: #808080;
          --color-table-border-btm: #808080;
      }
      body {
          font-family: Verdana, sans-serif;
          color: var(--color-fg);
          background-color: var(--color-bg);
          margin: 0px;
          padding: 0px;
      }
      .content {
          padding: 20px;
      }
      html {
          scroll-padding-top: 4rem;
      }
      h1 {
          color: #25915b;
          font-weight: bold;
          padding-bottom: 10px;
          padding-top: 30px;
      }
      h2 {
          color: #25915b;
          font-weight: bold;
      }
      h3 {
          color: #25915b;
          font-weight: bold;
      }

      /*Table of contents*/
      .toc {
          padding-bottom: 10px;
      }
      .toc a {
          color: var(--color-fg);
          text-decoration: none;
          border-bottom: 1px solid;
      }
      .toc ul li {
          font-size: 18;
          line-height: 1.5;
          list-style-type: none;
      }

      /*SCPI Commands tables*/
      .DataTableDiv {
          box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);
          padding: 10px 0 15px 15px;
          margin-top: 15px;
          margin-bottom: 15px;
      }  
      .DataTableName {
          font-size: large;
      }
      .DataTable {
          border-collapse: collapse;
          font-size: medium;
          min-width: 300px;
          text-align: left;
          border: thin solid var(--color-table-border);
      }
      .DataTable tr th{
          background-color: #206040;
          color: #ffffff;
          padding: 12px 15px;
      }
      .DataTable tr td{
          padding: 12px 15px;
      }
      .DataTable tbody tr:nth-of-type(odd) {
          background-color: var(--color-table-tr)
      }
      .DataTable tbody tr:last-of-type {
          border-bottom: var(--color-table-border-thickness, thin) solid var(--color-table-border-btm);
      }

      /*header*/
      .logo {
          height: 50px;
          margin-top: 5px;
          margin-bottom: 5px;
      }
      header {
          display: flex;
          justify-content: space-between;
          align-items: center;
          background-color: #c4c4c4;
          position: sticky;
          color: black;
          top: 0;
          font-weight: bold;
          font-size: 200%;
          font-family: "Ubuntu", Arial, Helvetica, sans-serif;
      }
      .toggleThemeButton {
          border: 1px solid var(--color-fg);
          padding: .5rem;
          background: var(--color-fg);
          border-radius: 10px;
          color: var(--color-bg);
          margin-right: .5rem;
          cursor: pointer;
      }
      .toggleThemeButton:hover {
          opacity: 0.7;
      }
      @media print {
          #toggleThemeButton {
          display: none;
          }
      }

      footer {
          position: fixed;
          left: 0;
          bottom: 0;
          width: 100%;
          background-color: #c4c4c4;
          color: black;
          text-align: left;
          font-size: 75%;
      }
    </style>
  </head>

  <body>

  <header>
    <xsl:call-template name="ZeraLogo"/>
    <xsl:call-template name="HeaderGlobal">
      <xsl:with-param name="adjustmentData" select="$adjustmentOutput"/>
    </xsl:call-template>
    <button id="toggleThemeButton" class="toggleThemeButton" onclick="toggleTheme()">Toggle theme</button>
  </header>

  <footer>
    <i>Last updated on <b><xsl:call-template name="CurrentDate"/></b></i>
  </footer>

  <script>
  function toggleTheme() {
    var element = document.body;
    element.classList.toggle("dark-theme");
  }
  </script>

  <!--In this section, we find out which chapters are present for the input xml file and assign them a number.
    Not every chapter is present in every mt310s2/com session-->
  <xsl:variable name="DFTFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'DFT'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="DFTChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="0"/>
      <xsl:with-param name="ChapterAvaialble" select="$DFTFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="PhaseValuesHeading" select="concat($MeasSystemsChapterNo, '.', $DFTChapterNo, ' ', $PhaseValues)"/>

  <xsl:variable name="POWFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'POW'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="POWChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$DFTChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$POWFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="PowerMeasurementHeading" select="concat($MeasSystemsChapterNo, '.', $POWChapterNo, ' ', $PowerMeasurement)"/>

  <xsl:variable name="FFTFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'FFT'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="FFTChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$POWChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$FFTFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="FFTValuesHeading" select="concat($MeasSystemsChapterNo, '.', $FFTChapterNo, ' ', $FFTValues)"/>

  <xsl:variable name="RMSFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'RMS'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="RMSChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$FFTChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$RMSFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="RMSValuesHeading" select="concat($MeasSystemsChapterNo, '.', $RMSChapterNo, ' ', $RMSValues)"/>

  <xsl:variable name="THDFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'THD'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="THDChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$RMSChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$THDFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="ThdMeasurementHeading" select="concat($MeasSystemsChapterNo, '.', $THDChapterNo, ' ', $ThdMeasurement)"/>

  <xsl:variable name="TRFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'TR'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="TRChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$THDChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$TRFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="TRValuesHeading" select="concat($MeasSystemsChapterNo, '.', $TRChapterNo, ' ', $TRValues)"/>

  <xsl:variable name="BDFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'BD'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="BDChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$TRChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$BDFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="BurdenValuesHeading" select="concat($MeasSystemsChapterNo, '.', $BDChapterNo, ' ', $BurdenValues)"/>
  
  <xsl:variable name="LAMFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'LAM'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="LAMChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$BDChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$LAMFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="LambdaValuesHeading" select="concat($MeasSystemsChapterNo, '.', $LAMChapterNo, ' ', $LambdaValues)"/>
  
  <xsl:variable name="PWFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'PW'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="PWChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$LAMChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$PWFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="HarmonicPowerValuesHeading" select="concat($MeasSystemsChapterNo, '.', $PWChapterNo, ' ', $HarmonicPowerValues)"/>

  <xsl:variable name="OSCFound">
    <xsl:call-template name="MeasurementNodeSearch">
      <xsl:with-param name="MeasSystem" select="'OSC'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="OSCChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$PWChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$OSCFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="OSCValuesHeading" select="concat($MeasSystemsChapterNo, '.', $OSCChapterNo, ' ', $OSCValues)"/>
  
  <xsl:variable name="SAMFound">
    <xsl:call-template name="SamplingConfigsNodeSearch"/>
  </xsl:variable>
  <xsl:variable name="SAMChapterNo">
    <xsl:call-template name="getChapterNumber">
      <xsl:with-param name="LastChapterNumber" select="$OSCChapterNo"/>
      <xsl:with-param name="ChapterAvaialble" select="$SAMFound"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="SamplingConfigsHeading" select="concat($MeasSystemsChapterNo, '.', $SAMChapterNo, ' ', $SamplingConfigs)"/>
  
  <div class="content">
    <p>This document was created with operating system version: <b><xsl:value-of select="$zenuxVersion"/></b>.</p>
    <!--Table of contents.-->
    <div class="toc">
      <h1 style="padding-top:0px;">Contents</h1>
      <ul>
          <li><a href="#ScpiStandard"><xsl:value-of select="$ScpiStandard"/></a></li>
          <li><a href="#DeviceConnection"><xsl:value-of select="$DeviceConnection"/></a></li>
          <li><a href="#DeviceInfo"><xsl:value-of select="$DeviceInfo"/></a></li>
          <li><a href="#DevIface"><xsl:value-of select="$DevIface"/></a></li>
          <li><a href="#SystemCmds"><xsl:value-of select="$SystemCmds"/></a></li>
          <li><a href="#ScpiStandardCmds"><xsl:value-of select="$ScpiStandardCmds"/></a></li>
          <xsl:if test="$adjustmentOutput != 'true'">
            <li><a href="#MeasSystems"><xsl:value-of select="$MeasSystems"/></a></li>
              <ul>
                <xsl:if test="$DFTFound != ''"><li><a href="#PhaseValues"><xsl:value-of select="$PhaseValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$POWFound != ''"><li><a href="#PowerMeasurement"><xsl:value-of select="$PowerMeasurementHeading"/></a></li></xsl:if>
                <xsl:if test="$FFTFound != ''"><li><a href="#FFTValues"><xsl:value-of select="$FFTValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$RMSFound != ''"><li><a href="#RMSValues"><xsl:value-of select="$RMSValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$THDFound != ''"><li><a href="#ThdMeasurement"><xsl:value-of select="$ThdMeasurementHeading"/></a></li></xsl:if>
                <xsl:if test="$TRFound != ''"><li><a href="#TRValues"><xsl:value-of select="$TRValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$BDFound != ''"><li><a href="#BurdenValues"><xsl:value-of select="$BurdenValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$LAMFound != ''"><li><a href="#LambdaValues"><xsl:value-of select="$LambdaValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$PWFound != ''"><li><a href="#HarmonicPowerValues"><xsl:value-of select="$HarmonicPowerValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$OSCFound != ''"><li><a href="#OSCValues"><xsl:value-of select="$OSCValuesHeading"/></a></li></xsl:if>
                <xsl:if test="$SAMFound != ''"><li><a href="#SamplingConfigs"><xsl:value-of select="$SamplingConfigsHeading"/></a></li></xsl:if>
              </ul>
            <li><a href="#Ranges"><xsl:value-of select="$Ranges"/></a></li>
            <li><a href="#ErrorCalculators"><xsl:value-of select="$ErrorCalculators"/></a></li>
          </xsl:if>
          <xsl:if test="$adjustmentOutput = 'true'">
            <li><a href="#Adjustment"><xsl:value-of select="$Adjustment"/></a></li>
          </xsl:if>
      </ul>
    </div>

    <!--Contents of document-->
    <h1 id="ScpiStandard"><xsl:value-of select="$ScpiStandard"/></h1>
    <p><xsl:copy-of select="document(concat($ProsaFolder, 'scpi-standard.html'))"/></p>

    <h1 id="DeviceConnection"><xsl:value-of select="$DeviceConnection"/></h1>
    <p><xsl:copy-of select="document(concat($ProsaFolder, 'dev-connection.html'))"/></p>

    <h1 id="DeviceInfo"><xsl:value-of select="$DeviceInfo"/></h1>
    <xsl:call-template name="DeviceInfo"/>

    <h1 id="DevIface"><xsl:value-of select="$DevIface"/></h1>
    <xsl:call-template name="DevIface">
        <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'dev-iface.html'))"/>
    </xsl:call-template>

    <h1 id="SystemCmds"><xsl:value-of select="$SystemCmds"/></h1>
    <xsl:call-template name="SystemCmds"/>

    <h1 id="ScpiStandardCmds"><xsl:value-of select="$ScpiStandardCmds"/></h1>
    <xsl:call-template name="ScpiStandardCmds"/>
    <xsl:call-template name="SystemInfo"/>

    <xsl:if test="$adjustmentOutput != 'true'">
    
      <h1 id="MeasSystems"><xsl:value-of select="$MeasSystems"/></h1>
      <p><xsl:copy-of select="document(concat($ProsaFolder, 'measurement-system.html'))"/></p>

      <!-- DFT phase values -->
      <xsl:if test="$DFTFound != ''">
        <h2 id="PhaseValues"><xsl:value-of select="$PhaseValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'DFT'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
          <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'measurement-DFT.html'))"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Power modules -->
      <xsl:if test="$POWFound != ''">
        <h2 id="PowerMeasurement"><xsl:value-of select="$PowerMeasurementHeading"/></h2>
        <xsl:variable name="prosaFile">
          <xsl:call-template name="PowMeasProsaFile">
            <xsl:with-param name="xmlFilePath" select="$sessionXml"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'POW'"/>
          <xsl:with-param name="PerSystemHeader" select="$PowerModule"/>
          <xsl:with-param name="Prosa" select="document($prosaFile)"/>
          <xsl:with-param name="CatalogPresent" select="'true'"/>
        </xsl:call-template>
      </xsl:if>

      <!-- FFT values -->
      <xsl:if test="$FFTFound != ''">
        <h2 id="FFTValues"><xsl:value-of select="$FFTValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'FFT'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
          <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'measurement-FFT.html'))"/>
        </xsl:call-template>
      </xsl:if>

      <!-- RMS Values -->
      <xsl:if test="$RMSFound != ''">
        <h2 id="RMSValues"><xsl:value-of select="$RMSValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'RMS'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
          <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'measurement-RMS.html'))"/>
        </xsl:call-template>
      </xsl:if>

      <!-- THD Measurement -->
      <xsl:if test="$THDFound != ''">
        <h2 id="ThdMeasurement"><xsl:value-of select="$ThdMeasurementHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'THD'"/>
          <xsl:with-param name="PerSystemHeader" select="$ThdModule"/>
          <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'measurement-THD.html'))"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Transformer Ratio Values -->
      <xsl:if test="$TRFound != ''">
        <h2 id="TRValues"><xsl:value-of select="$TRValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'TR'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Burden values -->
      <xsl:if test="$BDFound != ''">
        <h2 id="BurdenValues"><xsl:value-of select="$BurdenValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'BD'"/>
          <xsl:with-param name="PerSystemHeader" select="$Burden"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Lambda values -->
      <xsl:if test="$LAMFound != ''">
        <h2 id="LambdaValues"><xsl:value-of select="$LambdaValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'LAM'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Harmonic power values -->
      <xsl:if test="$PWFound != ''">
        <h2 id="HarmonicPowerValues"><xsl:value-of select="$HarmonicPowerValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'PW'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Oscillator values -->
      <xsl:if test="$OSCFound != ''">
        <h2 id="OSCValues"><xsl:value-of select="$OSCValuesHeading"/></h2>
        <xsl:call-template name="Measurement">
          <xsl:with-param name="MeasSystem" select="'OSC'"/>
          <xsl:with-param name="PerSystemHeader" select="''"/>
          <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'measurement-OSC.html'))"/>
        </xsl:call-template>
      </xsl:if>

      <!-- Sampling values -->
      <xsl:if test="$SAMFound != ''">
        <h1 id="SamplingConfigs"><xsl:value-of select="$SamplingConfigsHeading"/></h1>
        <xsl:call-template name="SamplingConfigs"/>
      </xsl:if>

      <h1 id="Ranges"><xsl:value-of select="$Ranges"/></h1>
      <p><xsl:copy-of select="document(concat($ProsaFolder, 'sense-range.html'))"/></p>
      <xsl:call-template name="SenseRange">
          <xsl:with-param name="OverloadProsa" select="document(concat($ProsaFolder, 'sense-range-overload.html'))"/>
      </xsl:call-template>
      <xsl:call-template name="Measurement">
        <xsl:with-param name="MeasSystem" select="'RNG'"/>
        <xsl:with-param name="PerSystemHeader" select="''"/>
      </xsl:call-template>

      <h1 id="ErrorCalculators"><xsl:value-of select="$ErrorCalculators"/></h1>
      <xsl:call-template name="ErrorCalculators">
          <xsl:with-param name="Prosa" select="document(concat($ProsaFolder, 'error-calculators.html'))"/>
          <xsl:with-param name="ProsaSec1" select="document(concat($ProsaFolder, 'error-calculators-sec1.html'))"/>
          <xsl:with-param name="ProsaSec2" select="document(concat($ProsaFolder, 'error-calculators-sec2.html'))"/>
          <xsl:with-param name="ProsaConfig" select="document(concat($ProsaFolder, 'error-calculators-config.html'))"/>
      </xsl:call-template>
    </xsl:if>

    <xsl:if test="$adjustmentOutput = 'true'">
      <h1 id="Adjustment"><xsl:value-of select="$Adjustment"/></h1>
      <p><xsl:copy-of select="document(concat($ProsaFolder, 'adjustment.html'))"/></p>
      <xsl:call-template name="Adjustment"/>
    </xsl:if>

  </div>
  </body>
  </html>
</xsl:template>

</xsl:stylesheet>
