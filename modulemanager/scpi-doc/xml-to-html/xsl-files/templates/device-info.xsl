<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="DeviceInfo">
  <div class="DataTableDiv">
    <p class="DataTableName"><b><xsl:value-of select="$Status"/></b></p>
    <table class="DataTable">
      <!-- table header -->
      <tr>
        <th><xsl:value-of select="$Description"/></th>
        <th><xsl:value-of select="$ScpiQuery"/></th>
      </tr>
      <!-- table data -->
      <xsl:for-each select="MODELS/STATUS">
        <xsl:for-each select="node()/VERSION/* | node()/*">
          <xsl:sort select="@Description"/>
          <xsl:if test="@Description!=''">
            <tr>
              <td><xsl:value-of select="@Description"/></td>
              <td><xsl:value-of select="@ScpiPath"/>?</td>
            </tr>
          </xsl:if>
        </xsl:for-each>
      </xsl:for-each>
    </table>
  </div>
</xsl:template>

</xsl:stylesheet>
