<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="SamplingConfigsNodeSearch">
  <xsl:for-each select="MODELS/CONFIGURATION/*">
    <xsl:sort select="local-name()"/>
    <xsl:if test="starts-with(local-name(), 'SAM')">
        <xsl:value-of select="local-name()"/>
    </xsl:if>    
  </xsl:for-each>  
</xsl:template>

<xsl:template name="SamplingConfigs">
    <xsl:param name="Heading"/>
    <xsl:for-each select="MODELS/CONFIGURATION/*">
        <xsl:if test="starts-with(local-name(), 'SAM')">
            <div class="DataTableDiv">
                <table class="DataTable">
                    <!-- table header -->
                    <tr>
                    <th><xsl:value-of select="$Description"/></th>
                    <th><xsl:value-of select="$ScpiPath"/></th>
                    <th><xsl:value-of select="$MinMax"/></th>
                    <th><xsl:value-of select="$ValidPar"/></th>
                    </tr>
                    <!-- table data -->
                    <xsl:for-each select="*">
                    <xsl:sort select="@Description"/>
                    <tr>
                        <td><xsl:value-of select="@Description"/></td>
                        <td><xsl:value-of select="@ScpiPath"/></td>
                        <xsl:choose>
                        <xsl:when test="@Min!='' and @Max!=''">
                            <td><xsl:value-of select="@Min"/> / <xsl:value-of select="@Max"/></td>
                            <td></td>
                        </xsl:when>
                        <xsl:when test="@ValidPar!=''">
                            <td></td>
                            <td><xsl:value-of select="@ValidPar"/></td>
                        </xsl:when>
                        <xsl:otherwise>
                            <td></td>
                            <td></td>
                        </xsl:otherwise>
                        </xsl:choose>
                    </tr>
                    </xsl:for-each>
                </table>
            </div>
        </xsl:if>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>