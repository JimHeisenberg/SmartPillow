--sudo -u postgres psql

--DROP DATABASE "SmartPillowDB";
CREATE DATABASE "SmartPillowDB";

\c SmartPillowDB;

CREATE SEQUENCE public."UserTable_UserID_seq";
CREATE SEQUENCE public."TimeTable_TimeID_seq";
CREATE SEQUENCE public."TimeTable_UserID_seq";
CREATE SEQUENCE public."DeviceTable_DeviceID_seq";
CREATE SEQUENCE public."DeviceTable_UserID_seq";
CREATE SEQUENCE public."DataTable_DataID_seq";
CREATE SEQUENCE public."DataTable_DeviceID_seq";
CREATE SEQUENCE public."TurnTable_UserID_seq";
CREATE SEQUENCE public."SleepingTable_UserID_seq";

CREATE SEQUENCE public."DataTableV2_DataID_seq";
CREATE SEQUENCE public."DataTableV2_DeviceID_seq";

CREATE TABLE public."UserTable"
(
    "UserID" integer NOT NULL DEFAULT nextval('"UserTable_UserID_seq"'::regclass),
    "UserName" character varying(30) COLLATE pg_catalog."default" NOT NULL,
    "Password" character varying(30) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT "UserTable_pkey" PRIMARY KEY ("UserID"),
    CONSTRAINT "UserTable_UserName_key" UNIQUE ("UserName")
);

CREATE TABLE public."TimeTable"
(
    "TimeID" integer NOT NULL DEFAULT nextval('"TimeTable_TimeID_seq"'::regclass),
    "UserID" integer NOT NULL DEFAULT nextval('"TimeTable_UserID_seq"'::regclass),
    "WakeupTime" time without time zone NOT NULL,
    "SleepingTime" time without time zone NOT NULL,
    CONSTRAINT "TimeTable_pkey" PRIMARY KEY ("TimeID"),
    CONSTRAINT "TimeTable_UserID_fkey" FOREIGN KEY ("UserID")
        REFERENCES public."UserTable" ("UserID") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE RESTRICT
);

CREATE TABLE public."DeviceTable"
(
    "DeviceID" integer NOT NULL DEFAULT nextval('"DeviceTable_DeviceID_seq"'::regclass),
    "UserID" integer NOT NULL DEFAULT nextval('"DeviceTable_UserID_seq"'::regclass),
    "DeviceName" character varying(30) COLLATE pg_catalog."default",
    CONSTRAINT "DeviceTable_pkey" PRIMARY KEY ("DeviceID"),
    CONSTRAINT "DeviceTable_UserID_fkey" FOREIGN KEY ("UserID")
        REFERENCES public."UserTable" ("UserID") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE RESTRICT
);

CREATE TABLE public."DataTable"
(
    "DataID" integer NOT NULL DEFAULT nextval('"DataTable_DataID_seq"'::regclass),
    "DeviceID" integer NOT NULL DEFAULT nextval('"DataTable_DeviceID_seq"'::regclass),
    "IsSleeping" boolean,
    "DateTime" timestamp without time zone,
    "Pressure" double precision,
    "Volume" double precision,
    "enable" int default 1,
    CONSTRAINT "DataTable_pkey" PRIMARY KEY ("DataID"),
    CONSTRAINT "DataTable_DeviceID_fkey" FOREIGN KEY ("DeviceID")
        REFERENCES public."DeviceTable" ("DeviceID") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE RESTRICT
);


CREATE TABLE public."DataTable_V2"
(
    "DataID" integer NOT NULL DEFAULT nextval('"DataTableV2_DataID_seq"'::regclass),
    "DeviceID" integer NOT NULL DEFAULT nextval('"DataTableV2_DeviceID_seq"'::regclass),
    "IsSleeping" boolean,
    "enable" int default 1,
    CONSTRAINT "DataTableV2._pkey" PRIMARY KEY ("DataID"),
    CONSTRAINT "DataTableV2_DeviceID_fkey" FOREIGN KEY ("DeviceID")
        REFERENCES public."DeviceTable" ("DeviceID") MATCH SIMPLE
        ON UPDATE CASCADE
        ON DELETE RESTRICT
);

CREATE TABLE public."TurnTable"
(
    "UserID" integer NOT NULL DEFAULT nextval('"TurnTable_UserID_seq"'::regclass),
    "Date" date,
    "TurnCount" integer,
    CONSTRAINT "TurnTable_pkey" PRIMARY KEY ("UserID", "Date"),
    CONSTRAINT "TurnTable_UserID_fkey" FOREIGN KEY ("UserID")
    REFERENCES public."UserTable" ("UserID") MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE RESTRICT
);

CREATE TABLE public."SleepingTable"
(
    "UserID" integer NOT NULL DEFAULT nextval('"SleepingTable_UserID_seq"'::regclass),
    "Date" date,
    "SleepTime" double precision,
    CONSTRAINT "SleepingTable_pkey" PRIMARY KEY ("UserID", "Date"),
    CONSTRAINT "SleepingTable_UserID_fkey" FOREIGN KEY ("UserID")
    REFERENCES public."UserTable" ("UserID") MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE RESTRICT
);

-- quit
\q
