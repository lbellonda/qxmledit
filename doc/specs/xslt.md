Specs for xslt execution


Menus:
Run an xslt on this file
Exec this xslt on a file

The menues open a panel:


source: {editor/file}
xslt: {editor/file}
output: {editor/file}

params: list of params:name/value. The params are red from the xslt file and can be edited.
The params are saved under a name.

Note that the output can be written to temp file.

The panel shares logic with anonymize.

The invocation can be done also using command line:

-execxslt -i input -x sheet -o output -pname=value (parameters)


