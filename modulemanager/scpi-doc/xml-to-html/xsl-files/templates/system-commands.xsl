<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="SystemCmds">
    <div class="DataTableDiv">
        <table class="DataTable">
            <!-- table header -->
            <tr>
                <th><xsl:value-of select="$Description"/></th>
                <th><xsl:value-of select="$ScpiPath"/></th>
                <th><xsl:value-of select="$ValidPar"/></th>
                <th><xsl:value-of select="$DataType"/></th>
            </tr>
            <xsl:for-each select="MODELS/CONFIGURATION/SYST/*">
                <!-- table data -->
                <tr>
                    <td><xsl:value-of select="@Description"/></td>
                    <td><xsl:value-of select="@ScpiPath"/></td>
                    <td><xsl:value-of select="@ValidPar"/></td>
                    <td><xsl:value-of select="@DataType"/></td>
                </tr>
            </xsl:for-each>
        </table>
    </div>
</xsl:template>
</xsl:stylesheet>
