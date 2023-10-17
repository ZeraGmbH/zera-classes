<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="GetXmlPath">
    <xsl:for-each select="parent::*">
        <xsl:call-template name="GetXmlPath"/>
    </xsl:for-each>
    <xsl:text>/</xsl:text>
    <xsl:value-of select="name()"/>
</xsl:template>

<xsl:template name="_GetXmlPath">
    <xsl:for-each select="parent::*">
        <xsl:call-template name="_GetXmlPath"/>
    </xsl:for-each>
    <xsl:value-of select="name()"/>
    <xsl:text>/</xsl:text>
</xsl:template>

</xsl:stylesheet>