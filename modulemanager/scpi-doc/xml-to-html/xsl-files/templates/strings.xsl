<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- default strings - translations?-->

<!-- common strings -->
<xsl:variable name="GlobalHeaderAppend" select="'SCPI commands'"/>
<xsl:variable name="Description" select="'Description'"/>
<xsl:variable name="ScpiPath" select="'SCPI path'"/>
<xsl:variable name="ScpiQuery" select="'SCPI query'"/>
<xsl:variable name="ScpiCmdType" select="'SCPI command type'"/>
<xsl:variable name="MinMax" select="'Min / Max'"/>
<xsl:variable name="ValidPar" select="'Valid strings'"/>
<xsl:variable name="RegEx" select="'RegEx'"/>
<xsl:variable name="ProsaFolder" select="'../html-prosa/'"/>
<xsl:variable name="Configuration" select="'Configuration'"/>
<xsl:variable name="Values" select="'Values'"/>
<xsl:variable name="Unit" select="'Unit'"/>
<xsl:variable name="DataType" select="'Data Type'"/>

<!-- Main chapter headings -->
<xsl:variable name="ScpiStandard" select="'1. SCPI Standard'"/>
<xsl:variable name="DeviceConnection" select="'2. Device connection'"/>
<xsl:variable name="DeviceInfo" select="'3. Device information'"/>
<xsl:variable name="DevIface" select="'4. Device interface'"/>
<xsl:variable name="ScpiStandardCmds" select="'5. Standard SCPI commands'"/>
<xsl:variable name="MeasSystemsChapterNo" select="'6'"/>
<xsl:variable name="MeasSystems" select="concat($MeasSystemsChapterNo, '. ', 'Measurement systems')"/>
<xsl:variable name="Ranges" select="'7. Ranges'"/>
<xsl:variable name="ErrorCalculators" select="'8. Comparison Measurements'"/>
<xsl:variable name="Adjustment" select="'6. Adjustment Commands'"/>

<!-- Measurement sub-chapter headings -->
<xsl:variable name="PhaseValues" select="'Phase amplitudes and angles'"/>
<xsl:variable name="PowerMeasurement" select="'Power measurement'"/>
<xsl:variable name="FFTValues" select="'FFT/DC values'"/>
<xsl:variable name="RMSValues" select="'RMS values'"/>
<xsl:variable name="ThdMeasurement" select="'THD measurement'"/>
<xsl:variable name="TRValues" select="'Transformer measurement'"/>
<xsl:variable name="BurdenValues" select="'Burden  measurement'"/>
<xsl:variable name="LambdaValues" select="'Lambda values'"/>
<xsl:variable name="HarmonicPowerValues" select="'Harmonic power values'"/>
<xsl:variable name="OSCValues" select="'Oscilloscope values'"/>
<xsl:variable name="SamplingConfigs" select="'Sampling/PLL configurations'"/>

<!-- Section headings -->
<xsl:variable name="PulseComparison" select="'Pulse comparison'"/>
<xsl:variable name="MeterTest" select="'Meter test'"/>
<xsl:variable name="EnergyComparison" select="'Energy comparison'"/>
<xsl:variable name="EnergyRegister" select="'Energy register'"/>
<xsl:variable name="PowerRegister" select="'Power register'"/>
<xsl:variable name="Status" select="'Status information'"/>
<xsl:variable name="PowerModule" select="'Power'"/>
<xsl:variable name="RangeSelection" select="'Selection'"/>
<xsl:variable name="ThdModule" select="'THD'"/>
<xsl:variable name="Burden" select="'BD'"/>
<xsl:variable name="Catalog" select="'Catalog'"/>
<xsl:variable name="Overload" select="'Overload'"/>
<xsl:variable name="AccumulatorInfo" select="'Accumulator information'"/>
<xsl:variable name="ErrorInfo" select="'Error information'"/>
<xsl:variable name="ModuleNumber" select="'Module number'"/>


</xsl:stylesheet>
