ExDestination = 100

function initiator_read(message)
    MsgType = message:MsgType()
    log_info("initiator read " .. MsgType)
    if MsgType == "D" then 
        message:fields():set(ExDestination, "DARK", set_operation.replace_first)
    end
end

function acceptor_read(message)
    msgtype = message:MsgType()
    log_info("initiator read " .. msgtype)
end