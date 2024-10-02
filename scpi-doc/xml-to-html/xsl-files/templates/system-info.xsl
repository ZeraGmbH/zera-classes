<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="SystemInfo">

    <xsl:for-each select="MODELS/SYSTEM/*">

        <xsl:if test="starts-with(local-name(), 'ERR')">
            <div class="DataTableDiv">
                <p class="DataTableName"><b><xsl:value-of select="$ErrorInfo"/></b></p>
                <table class="DataTable">
                    <!-- table header -->
                    <tr>
                        <th><xsl:value-of select="$ScpiQuery"/></th>
                        <th><xsl:value-of select="$Description"/></th>
                    </tr>
                    <!-- table data -->
                    <tr><td><xsl:value-of select="@ScpiPath"/>?</td>
                        <td><xsl:value-of select="@Description"/></td>
                    </tr>
                    <xsl:for-each select="*">
                        <tr>
                            <td><xsl:value-of select="@ScpiPath"/>?</td>
                            <td><xsl:value-of select="@Description"/></td>
                        </tr>
                    </xsl:for-each>
                </table>
            </div>
        </xsl:if>

    </xsl:for-each>

</xsl:template>

</xsl:stylesheet>