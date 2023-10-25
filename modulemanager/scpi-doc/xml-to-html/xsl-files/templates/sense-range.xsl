<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="SenseRange">
  <xsl:param name="OverloadProsa"/>

    <xsl:for-each select="MODELS/SENSE/*">
        <xsl:if test="starts-with(local-name(), 'RNG1')">
            <div class="DataTableDiv">
                <p class="DataTableName"><b><xsl:value-of select="$RangeSelection"/></b></p>
                <table class="DataTable">
                    <!-- table header -->
                    <tr>
                    <th><xsl:value-of select="$Description"/></th>
                    <th><xsl:value-of select="$ScpiPath"/></th>
                    <th><xsl:value-of select="$ValidPar"/></th>
                    <th><xsl:value-of select="$DataType"/></th>
                    <th><xsl:value-of select="$Unit"/></th>
                    </tr>
                    <!-- table data -->
                    <xsl:for-each select="*/*">
                        <xsl:if test="(local-name() = 'RANGE')">
                            <tr>
                                <td><xsl:value-of select="@Description"/></td>
                                <td><xsl:value-of select="@ScpiPath"/></td>
                                <td><xsl:value-of select="@ValidPar"/></td>
                                <td><xsl:value-of select="@DataType"/></td>
                                <td><xsl:value-of select="@Unit"/></td>
                            </tr>
                        </xsl:if>
                    </xsl:for-each>
                </table>
            </div>

            <div class="DataTableDiv">
                <p class="DataTableName"><b><xsl:value-of select="$Catalog"/></b></p>
                <table class="DataTable">
                    <!-- table header -->
                    <tr><th><xsl:value-of select="$ScpiQuery"/></th></tr>
                    <!-- table data -->
                    <xsl:for-each select="*/*/*">
                        <tr><td><xsl:value-of select="@ScpiPath"/>?</td></tr>
                    </xsl:for-each>
                </table>
            </div>

            <div class="DataTableDiv">
                <p class="DataTableName"><b><xsl:value-of select="$Overload"/></b></p>
                <p><xsl:copy-of select="$OverloadProsa"/></p>
                    <table class="DataTable">
                    <!-- table header -->
                    <tr>
                        <th><xsl:value-of select="$Description"/></th>
                        <th><xsl:value-of select="$ScpiPath"/></th>
                        <th><xsl:value-of select="$MinMax"/></th>
                    </tr>
                    <!-- table data -->
                    <xsl:for-each select="*">
                        <xsl:if test="(local-name() = 'OVERLOAD')">
                            <tr>
                                <td><xsl:value-of select="@Description"/></td>
                                <td><xsl:value-of select="@ScpiPath"/></td>
                                <td><xsl:value-of select="@Min"/> / <xsl:value-of select="@Max"/></td>
                            </tr>
                        </xsl:if>
                    </xsl:for-each>
                </table>
            </div>
        </xsl:if>
    </xsl:for-each>

</xsl:template>
</xsl:stylesheet>