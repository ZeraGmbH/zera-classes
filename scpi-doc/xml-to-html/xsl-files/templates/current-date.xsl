<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:ex="http://exslt.org/dates-and-times"
                              extension-element-prefixes="ex">

<xsl:template name="CurrentDate">
    <xsl:variable name="date" select="ex:date-time()"/>
    <!-- day -->
    <xsl:value-of select="substring($date, 9, 2)"/>
    <xsl:text>-</xsl:text>
    <!-- month -->
    <xsl:variable name="m" select="substring($date, 6, 2)"/>
    <xsl:value-of select="substring('JanFebMarAprMayJunJulAugSepOctNovDec', 3*($m - 1)+1, 3)"/>
    <xsl:text>-</xsl:text>
    <!-- year -->
    <xsl:value-of select="substring($date, 1, 4)"/>
</xsl:template>

</xsl:stylesheet>