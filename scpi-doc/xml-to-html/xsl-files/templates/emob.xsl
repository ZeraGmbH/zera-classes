<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="Emob">
  <xsl:param name="Prosa"/>
    <p><xsl:copy-of select="$Prosa"/></p>
    <div class="DataTableDiv">
      <table class="DataTable">
        <!-- table header -->
        <tr>
          <th><xsl:value-of select="$Description"/></th>
          <th><xsl:value-of select="$ScpiPath"/></th>
          <th><xsl:value-of select="$ScpiCmdType"/></th>
          <th><xsl:value-of select="$ValidPar"/></th>
          <th><xsl:value-of select="$DataType"/></th>
        </tr>
        <!-- table data -->
        <xsl:for-each select="MODELS/EMOB">
          <xsl:for-each select="node()/*/* | node()/*">
            <xsl:sort select="@Description"/>
            <xsl:if test="@Description!=''">
              <tr>
                <td><xsl:value-of select="@Description"/></td>
                <td><xsl:value-of select="@ScpiPath"/></td>
                <td><xsl:value-of select="@Type"/></td>
                <td><xsl:value-of select="@ValidPar"/></td>
                <td><xsl:value-of select="@DataType"/></td>
              </tr>
            </xsl:if>
          </xsl:for-each>
        </xsl:for-each>
      </table>
    </div>
</xsl:template>

</xsl:stylesheet>
