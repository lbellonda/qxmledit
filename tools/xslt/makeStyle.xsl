<?xml version='1.0' encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
                xml:lang="en"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:local="http://www.w3.org/2005/xquery-local-functions"
                xmlns:xs="http://www.w3.org/2001/XMLSchema">


    <xsl:output method="xml" omit-xml-declaration="no" indent="yes" encoding="UTF-8"/>

    <xsl:template match="/">
        <style name="XSLT" description="XSLT mode" namespace="http://www.w3.org/1999/XSL/Transform">
            <xsl:comment select="'this is a QXmlEdit style file'" />

            <keywords>
                <xsl:for-each select="/root/token">
                    <keyword keyword="{@name}" idStyle="1"/>
                </xsl:for-each>
            </keywords>

            <styles>
             <style id="1" color="2030C0" bold="true"/>
            </styles>

            <ids>
                <id id="name" alpha="true"/>
                <id id="test" alpha="true"/>
                <id id="select" alpha=""/>
                <id id="match" alpha=""/>
            </ids>
        </style>
    </xsl:template>
</xsl:stylesheet>
