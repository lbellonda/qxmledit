Formatting modes
================

Insert meta:
 * is meta is not on
  * set meta = on
  * assign current formatting options
 * if there is a directive, it is updated
   * if the directive is up to date, do nothing
 * if there is no directive, it is inserted
Undo:
 undo insert or update
 reset formatting to previous values
 set meta to its previous value


Remove meta:
 * if flag is on
   * set flag off
   * values are untouched
 * if a directive is present
   * remove directive

Undo:
 * set flag to previous state
 * if the directive has been removed
   * insert directive


Enable management:
 * when reading
   * if exists directive
     * flag on
     * apply formatting
 * when writing
   * if flag on
     * apply formatting
     * if found directive
       * update directive
 * when new
   * insert metainfo if option is set


Managment:
<table border="1">
    <tr>
        <th></th>
        <th>Enabled</th>
        <th>NotEnabled</th>
    </tr>
    <tr>
        <th>Flag ON</th>
        <td>Mod val/ins</td>
        <td>non mod</td>
    </tr>
    <tr>
        <th>Flag OFF</th>
        <td>non mod/ins</td>
        <td>non mod</td>
    </tr>
</table>

Meta format:
?qxmledit type="formatting" indent="on/off" indentValue="n" sortAlphaAttr="on/off" attrLineLen="off/n"
Sample:
<?qxmledit type="formatting" indent="off" indentValue="0" sortAlphaAttr="off" attrLineLen="off"?>
