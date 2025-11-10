<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="NodeSearch">
  <xsl:param name="Node"/>
  <xsl:for-each select="MODELS/*">
    <xsl:sort select="local-name()"/>
    <xsl:if test="starts-with(local-name(), $Node)">
        <xsl:value-of select="local-name()"/>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>