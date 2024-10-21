<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="HeaderGlobal">
    <xsl:param name="sessionName"/>
    <xsl:param name="adjustmentData"/>
    <xsl:choose>
        <xsl:when test="$adjustmentData='true'">
            <xsl:text>Adjustment </xsl:text><xsl:value-of select="$GlobalHeaderAppend"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="DEVICE"/><xsl:text> - </xsl:text>
            <xsl:value-of select="$sessionName"/><xsl:text> session - </xsl:text>
            <xsl:value-of select="$GlobalHeaderAppend"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
