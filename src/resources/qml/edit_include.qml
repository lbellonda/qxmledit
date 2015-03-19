import Qt 4.7

 Rectangle {
     height: 100
     width: 400;

     property color buttonColor: "lightblue"
     property color onHoverColor: "gold"
     property color borderColor: "white"


/*     Text {
         id: typeSomething
         anchors.fill: parent; anchors.leftMargin: 8
         verticalAlignment: Text.AlignVCenter
         text: "id:"
         color: "gray"
         font.italic: true
     }
*/

     TextInput {
         id: textInput
         //anchors { left: parent.left; leftMargin: 8; right: clear.left; rightMargin: 8; verticalCenter: parent.verticalCenter }
         focus: true
         selectByMouse: true
         text: item.name;
     }

     Rectangle {
         x:0;
         width: 100; height: 25

         Text {
                  text: "save"
              }

         MouseArea {
                  id: saveButton
                  hoverEnabled: true
                  onEntered: parent.border.color = onHoverColor
                  onExited:  parent.border.color = borderColor
                  anchors.fill: parent

                  onClicked:{
                      item.name = textInput.text
                      console.log(" clicked" )
parent.color = 'blue'
                  }
              }
     }
 }
