<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="ScpiStandardCmds">
    <div class="DataTableDiv">
        <table class="DataTable">
            <!-- table header -->
            <tr>
                <th><xsl:value-of select="$ScpiPath"/></th>
                <th><xsl:value-of select="$ScpiCmdType"/></th>
                <th><xsl:value-of select="$Description"/></th>
            </tr>
            <xsl:for-each select="MODELS/*[not(*)]">
                <xsl:sort select="local-name()"/>
                <!-- table data -->
                <tr>
                    <td><xsl:value-of select="@ScpiPath"/></td>
                    <td><xsl:value-of select="substring-after(@Type, 'Model,')"/></td>
                    <td><xsl:value-of select="@Description"/></td>
                </tr>
            </xsl:for-each>
        </table>
    </div>
</xsl:template>
</xsl:stylesheet>