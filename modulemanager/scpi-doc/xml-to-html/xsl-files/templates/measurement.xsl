<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="MeasurementNodeSearch">
  <xsl:param name="MeasSystem"/>
  <xsl:for-each select="MODELS/MEASURE/*">
    <xsl:sort select="local-name()"/>
    <xsl:if test="starts-with(local-name(), $MeasSystem)">
        <xsl:value-of select="local-name()"/>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template name="Measurement">
  <xsl:param name="MeasSystem"/>
  <xsl:param name="PerSystemHeader"/>
  <xsl:param name="Prosa"/>
  <xsl:param name="CatalogPresent"/>
  <xsl:if test="$Prosa != ''">
    <p><xsl:copy-of select="$Prosa"/></p>
  </xsl:if>
  <xsl:for-each select="MODELS/MEASURE/*">
    <xsl:sort select="local-name()"/>
    <xsl:if test="starts-with(local-name(), $MeasSystem)">
      <xsl:if test="$PerSystemHeader != ''">
        <h3><xsl:value-of select="$PerSystemHeader"/><xsl:text> </xsl:text><xsl:value-of select="substring-after(local-name(), $MeasSystem)"/></h3>
      </xsl:if>
      <xsl:variable name="localName" select="local-name()"/>
      <!-- configuration table -->
      <xsl:for-each select="../../CONFIGURATION/*">
        <xsl:if test="contains(local-name(), $localName)">

          <div class="DataTableDiv">
            <p class="DataTableName"><b><xsl:value-of select="$Configuration"/></b></p>
            <table class="DataTable">
                <!-- table header -->
                <tr>
                  <th><xsl:value-of select="$Description"/></th>
                  <th><xsl:value-of select="$ScpiPath"/></th>
                  <th><xsl:value-of select="$MinMax"/></th>
                  <th><xsl:value-of select="$ValidPar"/></th>
                  <th><xsl:value-of select="$DataType"/></th>
                  <th><xsl:value-of select="$Unit"/></th>
                </tr>
                <!-- table data -->
                <xsl:for-each select="*">
                    <xsl:if test="@Description!=''">
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
                            <td><xsl:value-of select="@DataType"/></td>
                            <td><xsl:value-of select="@Unit"/></td>
                        </tr>
                    </xsl:if>
                    <xsl:for-each select="*">
                        <xsl:if test="@Description!=''">
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
                                <td><xsl:value-of select="@DataType"/></td>
                                <td><xsl:value-of select="@Unit"/></td>
                            </tr>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:for-each>
            </table>
          </div>
          <xsl:if test="$CatalogPresent != ''">
              <div class="DataTableDiv">
                <p class="DataTableName"><b><xsl:value-of select="$Catalog"/></b></p>
                <table class="DataTable">
                    <!-- table header -->
                    <tr><th><xsl:value-of select="$ScpiQuery"/></th></tr>
                    <!-- table data -->
                    <xsl:for-each select="*/*">
                        <tr><td><xsl:value-of select="@ScpiPath"/>?</td></tr>
                    </xsl:for-each>
                </table>
              </div>
          </xsl:if>
        </xsl:if>
      </xsl:for-each>

      <!-- value table -->
      <div class="DataTableDiv">
        <p class="DataTableName"><b><xsl:value-of select="$Values"/></b></p>
        <table class="DataTable">
            <!-- table header -->
            <tr>
              <th><xsl:value-of select="$Description"/></th>
              <th><xsl:value-of select="$ScpiQuery"/></th>
              <th><xsl:value-of select="$Unit"/></th>
            </tr>
            <!-- table data -->
            <xsl:for-each select="*">
                <tr>
                    <td><xsl:value-of select="@Description"/></td>
                    <td><xsl:value-of select="@ScpiPath"/>?</td>
                    <td><xsl:value-of select="@Unit"/></td>
                </tr>
            </xsl:for-each>
        </table>
      </div>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
