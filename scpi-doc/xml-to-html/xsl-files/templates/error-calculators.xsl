<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="ErrorCalculators">
  <xsl:param name="Prosa"/>
  <xsl:param name="ProsaSec1"/>
  <xsl:param name="ProsaSec2"/>
  <xsl:param name="ProsaConfig"/>
  <p><xsl:copy-of select="$Prosa"/></p>
  <xsl:for-each select="MODELS/CALCULATE/*">

    <xsl:sort select="local-name()"/>
    <!-- error calculator type specific headers -->
    <xsl:choose>
      <xsl:when test="contains(local-name(), 'EC')">
        <h2><xsl:value-of select="$PulseComparison"/></h2>
      </xsl:when>
      <xsl:when test="contains(local-name(), 'EM')">
        <h2><xsl:value-of select="$EnergyRegister"/></h2>
      </xsl:when>
      <xsl:when test="contains(local-name(), 'PM')">
        <h2><xsl:value-of select="$PowerRegister"/></h2>
      </xsl:when>
    </xsl:choose>

    <xsl:for-each select="*">
      <xsl:sort select="local-name()"/>
      <xsl:if test="starts-with(local-name(), 'N_')">
        <xsl:variable name="ErrorCalcNumber" select="local-name()"/>
        <xsl:if test="contains(local-name(..), 'EC')">
          <xsl:if test="contains(local-name(), '0001')">
              <p><xsl:copy-of select="$ProsaSec1"/></p>
          </xsl:if>
          <xsl:if test="contains(local-name(), '0002')">
              <p><xsl:copy-of select="$ProsaSec2"/></p>
          </xsl:if>
        </xsl:if>
        <div class="DataTableDiv">
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
                <td><xsl:value-of select="@DataType"/></td>
                <td><xsl:value-of select="@Unit"/></td>
              </tr>
            </xsl:for-each>
          </table>
        </div>

        <xsl:if test="contains(local-name(..), 'EC')">
          <!-- configuration table -->
          <xsl:for-each select="../../../CONFIGURATION/*">
            <xsl:if test="contains(local-name(), 'EC')">
              <div class="DataTableDiv">
                <p class="DataTableName"><b><xsl:value-of select="$Configuration"/></b></p>
                <p><xsl:copy-of select="$ProsaConfig"/></p>
                <table class="DataTable">
                    <!-- table header -->
                    <tr>
                      <th><xsl:value-of select="$Description"/></th>
                      <th><xsl:value-of select="$ScpiPath"/></th>
                      <th><xsl:value-of select="$ValidPar"/></th>
                      <th><xsl:value-of select="$RegEx"/></th>
                      <th><xsl:value-of select="$DataType"/></th>
                    </tr>
                    <!-- table data -->
                    <xsl:for-each select="*">
                      <xsl:if test="local-name() = $ErrorCalcNumber">
                        <xsl:for-each select="*">
                            <xsl:sort select="@Description"/>
                            <tr>
                                <td><xsl:value-of select="substring-before(substring-after(@Description, '('), ')')"/></td>
                                <td><xsl:value-of select="@ScpiPath"/></td>
                                <xsl:choose>
                                  <xsl:when test="@ValidPar!=''">
                                    <td><xsl:value-of select="@ValidPar"/></td>
                                    <td></td>
                                  </xsl:when>
                                  <xsl:when test="@RegEx!=''">
                                    <td></td>
                                    <td><xsl:value-of select="@RegEx"/></td>
                                  </xsl:when>
                                  <xsl:otherwise>
                                    <td></td>
                                    <td></td>
                                  </xsl:otherwise>
                                </xsl:choose>
                                <td><xsl:value-of select="@DataType"/></td>
                            </tr>
                        </xsl:for-each>
                      </xsl:if>
                    </xsl:for-each>
                </table>
              </div>
            </xsl:if>
          </xsl:for-each>
        </xsl:if>

      </xsl:if>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
