// CSHARP_MIGRATION: TODO:
// this is a temporary solution to use the constants defined in winmeta.h file

namespace CalculatorApp
{
    static class WinMeta
    {
        public const int WINEVENT_CHANNEL_CLASSIC_TRACE = 0x0;
        public const int WINEVENT_CHANNEL_GLOBAL_SYSTEM = 0x8;
        public const int WINEVENT_CHANNEL_GLOBAL_APPLICATION = 0x9;
        public const int WINEVENT_CHANNEL_GLOBAL_SECURITY = 0xa;
        public const int WINEVENT_CHANNEL_TRACELOGGING = 0xb;
        public const int WINEVENT_CHANNEL_PROVIDERMETADATA = 0xc;
        public const int WINEVENT_LEVEL_LOG_ALWAYS = 0x0;
        public const int WINEVENT_LEVEL_CRITICAL = 0x1;
        public const int WINEVENT_LEVEL_ERROR = 0x2;
        public const int WINEVENT_LEVEL_WARNING = 0x3;
        public const int WINEVENT_LEVEL_INFO = 0x4;
        public const int WINEVENT_LEVEL_VERBOSE = 0x5;
        public const int WINEVENT_LEVEL_RESERVED_6 = 0x6;
        public const int WINEVENT_LEVEL_RESERVED_7 = 0x7;
        public const int WINEVENT_LEVEL_RESERVED_8 = 0x8;
        public const int WINEVENT_LEVEL_RESERVED_9 = 0x9;
        public const int WINEVENT_LEVEL_RESERVED_10 = 0xa;
        public const int WINEVENT_LEVEL_RESERVED_11 = 0xb;
        public const int WINEVENT_LEVEL_RESERVED_12 = 0xc;
        public const int WINEVENT_LEVEL_RESERVED_13 = 0xd;
        public const int WINEVENT_LEVEL_RESERVED_14 = 0xe;
        public const int WINEVENT_LEVEL_RESERVED_15 = 0xf;
        public const int WINEVENT_OPCODE_INFO = 0x0;
        public const int WINEVENT_OPCODE_START = 0x1;
        public const int WINEVENT_OPCODE_STOP = 0x2;
        public const int WINEVENT_OPCODE_DC_START = 0x3;
        public const int WINEVENT_OPCODE_DC_STOP = 0x4;
        public const int WINEVENT_OPCODE_EXTENSION = 0x5;
        public const int WINEVENT_OPCODE_REPLY = 0x6;
        public const int WINEVENT_OPCODE_RESUME = 0x7;
        public const int WINEVENT_OPCODE_SUSPEND = 0x8;
        public const int WINEVENT_OPCODE_SEND = 0x9;
        public const int WINEVENT_OPCODE_RECEIVE = 0xf0;
        public const int WINEVENT_OPCODE_RESERVED_241 = 0xf1;
        public const int WINEVENT_OPCODE_RESERVED_242 = 0xf2;
        public const int WINEVENT_OPCODE_RESERVED_243 = 0xf3;
        public const int WINEVENT_OPCODE_RESERVED_244 = 0xf4;
        public const int WINEVENT_OPCODE_RESERVED_245 = 0xf5;
        public const int WINEVENT_OPCODE_RESERVED_246 = 0xf6;
        public const int WINEVENT_OPCODE_RESERVED_247 = 0xf7;
        public const int WINEVENT_OPCODE_RESERVED_248 = 0xf8;
        public const int WINEVENT_OPCODE_RESERVED_249 = 0xf9;
        public const int WINEVENT_OPCODE_RESERVED_250 = 0xfa;
        public const int WINEVENT_OPCODE_RESERVED_251 = 0xfb;
        public const int WINEVENT_OPCODE_RESERVED_252 = 0xfc;
        public const int WINEVENT_OPCODE_RESERVED_253 = 0xfd;
        public const int WINEVENT_OPCODE_RESERVED_254 = 0xfe;
        public const int WINEVENT_OPCODE_RESERVED_255 = 0xff;
        public const int WINEVENT_TASK_NONE = 0x0;
        public const int WINEVT_KEYWORD_ANY = 0x0;
        public const long WINEVENT_KEYWORD_RESPONSE_TIME = 0x1000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_49 = 0x2000000000000;
        public const long WINEVENT_KEYWORD_WDI_DIAG = 0x4000000000000;
        public const long WINEVENT_KEYWORD_SQM = 0x8000000000000;
        public const long WINEVENT_KEYWORD_AUDIT_FAILURE = 0x10000000000000;
        public const long WINEVENT_KEYWORD_AUDIT_SUCCESS = 0x20000000000000;
        public const long WINEVENT_KEYWORD_CORRELATION_HINT = 0x40000000000000;
        public const long WINEVENT_KEYWORD_EVENTLOG_CLASSIC = 0x80000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_56 = 0x100000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_57 = 0x200000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_58 = 0x400000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_59 = 0x800000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_60 = 0x1000000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_61 = 0x2000000000000000;
        public const long WINEVENT_KEYWORD_RESERVED_62 = 0x4000000000000000;
        public const ulong WINEVENT_KEYWORD_RESERVED_63 = 0x8000000000000000;
        public const long MSG_category_Devices = 0x00000001L;

        public const long MSG_category_Disk = 0x00000002L;

        public const long MSG_category_Network = 0x00000007L;

        public const long MSG_category_Printers = 0x00000003L;

        public const long MSG_category_Services = 0x00000004L;

        public const long MSG_category_Shell = 0x00000005L;

        public const long MSG_category_SystemEvent = 0x00000006L;

        public const long MSG_channel_Application = 0x00000100L;

        public const long MSG_channel_ProviderMetadata = 0x90000003L;

        public const long MSG_channel_Security = 0x00000101L;

        public const long MSG_channel_System = 0x00000102L;

        public const long MSG_channel_TraceClassic = 0x90000001L;

        public const long MSG_channel_TraceLogging = 0x90000002L;

        public const long MSG_keyword_AnyKeyword = 0x10000000L;

        public const long MSG_keyword_AuditFailure = 0x10000035L;

        public const long MSG_keyword_AuditSuccess = 0x10000036L;

        public const long MSG_keyword_Classic = 0x10000038L;

        public const long MSG_keyword_CorrelationHint = 0x10000037L;

        public const long MSG_keyword_ResponseTime = 0x10000031L;

        public const long MSG_keyword_SQM = 0x10000034L;

        public const long MSG_keyword_WDIDiag = 0x10000033L;

        public const long MSG_level_Critical = 0x50000001L;

        public const long MSG_level_Error = 0x50000002L;

        public const long MSG_level_Informational = 0x50000004L;

        public const long MSG_level_LogAlways = 0x50000000L;

        public const long MSG_level_Verbose = 0x50000005L;

        public const long MSG_level_Warning = 0x50000003L;

        public const long MSG_opcode_DCStart = 0x30030000L;

        public const long MSG_opcode_DCStop = 0x30040000L;

        public const long MSG_opcode_Extension = 0x30050000L;

        public const long MSG_opcode_Info = 0x30000000L;

        public const long MSG_opcode_Receive = 0x30F00000L;

        public const long MSG_opcode_Reply = 0x30060000L;

        public const long MSG_opcode_Resume = 0x30070000L;

        public const long MSG_opcode_Send = 0x30090000L;

        public const long MSG_opcode_Start = 0x30010000L;

        public const long MSG_opcode_Stop = 0x30020000L;

        public const long MSG_opcode_Suspend = 0x30080000L;

        public const long MSG_task_None = 0x70000000L;
    }
}
