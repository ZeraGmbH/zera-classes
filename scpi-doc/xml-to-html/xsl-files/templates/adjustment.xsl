<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="Adjustment">
  <xsl:for-each select="MODELS/CALCULATE/*">
    <xsl:if test="starts-with(local-name(), 'ADJ')">
      <div class="DataTableDiv">
        <table class="DataTable">
          <!-- table header -->
          <tr>
            <th><xsl:value-of select="$Description"/></th>
            <th><xsl:value-of select="$ScpiPath"/></th>
            <th><xsl:value-of select="$MinMax"/></th>
            <th><xsl:value-of select="$DataType"/></th>
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
                </xsl:when>
                <xsl:otherwise>
                    <td></td>
                </xsl:otherwise>
              </xsl:choose>
              <td><xsl:value-of select="@DataType"/></td>
            </tr>
          </xsl:for-each>
        </table>
      </div>
    </xsl:if>
    
  </xsl:for-each>

</xsl:template>

</xsl:stylesheet>
