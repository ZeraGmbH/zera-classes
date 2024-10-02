<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="get-file-name-from-path.xsl"/>
<xsl:include href="string-replace.xsl"/>

<xsl:template name="PowMeasProsaFile">
  <xsl:param name="xmlFilePath" />
  
  <xsl:variable name="xmlFileName">
    <xsl:call-template name="getFileNameFromPath">
      <xsl:with-param name="filePath" select="$xmlFilePath"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="htmlProsaPath" select="concat($ProsaFolder, 'measurement-POW/')"/>
  
  <xsl:variable name="htmlProsaFile">
    <xsl:call-template name="StringReplace">
      <xsl:with-param name="input" select="concat($htmlProsaPath, $xmlFileName)"/>
      <xsl:with-param name="search-string" select="'.xml'"/>
      <xsl:with-param name="replace-string" select="'.html'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:value-of select="$htmlProsaFile"/>

</xsl:template>

</xsl:stylesheet>