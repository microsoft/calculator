use std::sync::Arc;

pub enum Command {
    CommandNULL,
    CommandSIGN,
    CommandPNT,
    CommandEQU,
    CommandADD,
    CommandSUB,
    CommandMUL,
    CommandDIV,
    CommandPWR,
    CommandSQRT,
    CommandDIVINV,
    CommandPERCENT,
    CommandFRAC,
    CommandSQR,
    CommandCUB,
    CommandREC,
    CommandSIN,
    CommandCOS,
    CommandTAN,
    CommandLOG,
    CommandLN,
    CommandEXP,
    CommandPOWE,
    CommandPOW10,
    CommandASIN,
    CommandACOS,
    CommandATAN,
    CommandSINH,
    CommandCOSH,
    CommandTANH,
    CommandASINH,
    CommandACOSH,
    CommandATANH,
    CommandSEC,
    CommandCSC,
    CommandCOT,
    CommandASEC,
    CommandACSC,
    CommandACOT,
    CommandSECH,
    CommandCSCH,
    CommandCOTH,
    CommandASECH,
    CommandACSCH,
    CommandACOTH,
    CommandDEG,
    CommandRAD,
    CommandGRAD,
    CommandFE,
    CommandMCLEAR,
    CommandBACK,
    CommandEXP,
    CommandSTORE,
    CommandMPLUS,
    CommandMMINUS,
    CommandCLEAR,
    ModeBasic,
    ModeScientific,
    ModeProgrammer,
}

pub trait IExpressionCommand {
    fn execute(&self);
}

pub struct CCommand {
    command: Command,
    expression_command: Arc<dyn IExpressionCommand>,
}

impl CCommand {
    pub fn new(command: Command, expression_command: Arc<dyn IExpressionCommand>) -> Self {
        Self {
            command,
            expression_command,
        }
    }

    pub fn execute(&self) {
        self.expression_command.execute();
    }

    pub fn get_command(&self) -> Command {
        self.command
    }
}
