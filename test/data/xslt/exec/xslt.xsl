<?xml version='1.0' encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
                xml:lang="en"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:local="http://www.w3.org/2005/xquery-local-functions"
                xmlns:xs="http://www.w3.org/2001/XMLSchema">

	<xsl:param name="p1" select="'this is p1'"/>
	<xsl:param name="p2" select="'this is p2'"/>

    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>

	<xsl:template match="/">
		<out attr1="{$p1}" attr2="{$p2}">
        	<xsl:apply-templates select="*"/>
		</out>
	</xsl:template>
    <xsl:template match="a">
        <x>
            <xsl:attribute name="x" select="@aa" />
        </x>
    </xsl:template>

</xsl:stylesheet>

