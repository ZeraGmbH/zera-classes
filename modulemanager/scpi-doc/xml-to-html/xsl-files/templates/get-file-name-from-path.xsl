<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="getFileNameFromPath">
  <xsl:param name="filePath" />
  <xsl:choose>
    <xsl:when test="(substring-after($filePath,'/'))">
      <xsl:call-template name="getFileNameFromPath">
        <xsl:with-param name="filePath" select="substring-after($filePath,'/')" />
      </xsl:call-template>  
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$filePath" />
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>