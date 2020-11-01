# FIX Standards

These Orchestra files are snapshots of versions of the FIX protocol that are still supported. 

* FIX 4.2
* FIX 4.4
* FIX Latest

Orchestra files for incremental updates to the FIX standard, called extension packs, will be published periodically as FIX Latest. Extension pack files including gap analysis documents are available at [FIX Extension Packs](https://www.fixtrading.org/extension-packs/).

The FIX 4.2 and 4.4 files were translated from FIX Repository 2010 Edition into the Orchestra schema. Since the old Repository format only described message structures, these translations are limited to that content. 

This resource is available through Maven central repository as

```xml
<dependency>
  <groupId>io.fixprotocol.orchestrations</groupId>
  <artifactId>fix-standard</artifactId>
  <version>1.5.0</version>
</dependency>
```