<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="getChapterNumber">
    <xsl:param name="LastChapterNumber"/>
    <xsl:param name="ChapterAvaialble"/>

    <xsl:choose>
        <xsl:when test="$ChapterAvaialble!=''">
            <xsl:value-of select="$LastChapterNumber + 1"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$LastChapterNumber"/>
        </xsl:otherwise>
    </xsl:choose>
    
</xsl:template>
</xsl:stylesheet>