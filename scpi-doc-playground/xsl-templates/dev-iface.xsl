<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="DevIface">
  <xsl:param name="Prosa"/>
    <p><xsl:copy-of select="$Prosa"/></p>
    <div class="DataTableDiv">
        <table class="DataTable">
            <!-- table header -->
            <tr><th><xsl:value-of select="$ScpiQuery"/></th></tr>
            <xsl:for-each select="MODELS/DEVICE/*">
                <!-- table data -->
                <tr><td><xsl:value-of select="@ScpiPath"/>?</td></tr>
            </xsl:for-each>
        </table>
    </div>
</xsl:template>
</xsl:stylesheet>