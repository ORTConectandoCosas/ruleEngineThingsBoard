// codigos JS para los nodos de la cadena de nodos de TB

//******************************************************************************
//AMBIENT LIGHT DARK

return typeof msg.valorLDR === 'undefined' 
        || (msg.valorLDR >= metadata.shared_darkMin && msg.valorLDR <= metadata.shared_darkMax);


//******************************************************************************
//AMBIENT LIGHT NORMAL

return typeof msg.valorLDR === 'undefined' 
        || (msg.valorLDR >= metadata.shared_normalMin && msg.valorLDR <= metadata.shared_normalMax);


 //******************************************************************************
 //AMBIENT LIGHT BRIGHT

 return typeof msg.valorLDR === 'undefined' 
        || (msg.valorLDR >= metadata.shared_brightMin && msg.valorLDR <= metadata.shared_brightMax);

//******************************************************************************
//COMMAND LED ON

var msg = {"method":"lightON","params":metadata.shared_lightONCmdValue};
var metadata = { "requestId": "cmd"};
var msgType = "DebugMsg";

return { msg: msg, metadata: metadata, msgType: msgType };

//******************************************************************************
//COMMAND LED DIM

var msg = {"method":"lightDIM","params":metadata.shared_lightDIMCmdValue};
var metadata = { "requestId": "cmd"};
var msgType = "DebugMsg";

return { msg: msg, metadata: metadata, msgType: msgType };

//******************************************************************************
//COMMAND LED OFF

var msg = {"method":"lightOFF","params":metadata.shared_lightOFFCmdValue};
var metadata = { "requestId": "cmd"};
var msgType = "DebugMsg";

return { msg: msg, metadata: metadata, msgType: msgType };