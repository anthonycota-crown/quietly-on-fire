
/* ============================================================================
   ISR-5413 — HD6000 Family TDM Modules NOT on Target Firmware
   ----------------------------------------------------------------------------
   Requirement : All HD6000 (incl. HD6000c) TDM modules not on firmware
                 169044P10702001  (Tyler Dorsten, Nov 6 2025 ticket comment)
   Source      : LifecycleServicesReporting.dbo.truck_firmware_tracker  (LCSR)
                 + LifecycleServicesReporting.dbo.Truck_Master          (LCSR)
   System      : SQL Server (ILSQL1). Covers BOTH InfoLink4 + InfoLink6.
   Notes       : - truck_firmware_tracker has NO customer_name / location_name.
                   Both come from Truck_Master (company + location_name).
                 - Deduped to one row per physical module.
                 - Truck_Master has NO PK; deduped before join to prevent fan-out.
                 - Join: serial first, MAC fallback.
                 - "under this version" = "not exactly on target". CONFIRM w/ Tyler.
   ============================================================================ */

DECLARE @target_firmware varchar(50) = '169044P10702001';

/* ---- Step 1: filter the tracker to ticket criteria ---- */
WITH filtered AS (
    SELECT
        tft.id,
        tft.database_name,
        tft.mfr_serial_number,
        tft.source,
        tft.eqpmnt_model_name,
        tft.module_mac_address,
        tft.module_model,
        tft.last_comm_dt,
        tft.device_name,
        tft.device_type,
        tft.firmware_version,
        tft.boot_version,
        tft.hardware_version,
        tft.os_type,
        tft.os_version,
        tft.module_type,
        tft.update_dt_utc
    FROM LifecycleServicesReporting.dbo.truck_firmware_tracker AS tft
    WHERE
        -- CRITERION 1: HD6000 family (HD6000c included so Anthony can filter it out)
        tft.module_model IN ('HD6000', 'HD6000c')
        -- CRITERION 2: TDM modules only ("under this TDM version")
        AND tft.device_name = 'TDM'
        -- CRITERION 3: not on the target firmware build
        AND (tft.firmware_version <> @target_firmware
             OR tft.firmware_version IS NULL)
        -- Active records only
        AND tft.current_flag = 1
        AND tft.deleted_flag != 1
),

/* ---- Step 2: dedupe tracker to one row per physical module ---- */
ranked AS (
    SELECT
        filtered.*,
        ROW_NUMBER() OVER (
            PARTITION BY mfr_serial_number, module_mac_address
            ORDER BY
                CASE WHEN last_comm_dt IS NULL THEN 1 ELSE 0 END,
                last_comm_dt DESC,
                update_dt_utc DESC
        ) AS rn
    FROM filtered
),
tracker_deduped AS (
    SELECT * FROM ranked WHERE rn = 1
),

/* ---- Step 3: dedupe Truck_Master (no PK) to one row per serial ---- */
tm_deduped AS (
    SELECT
        tm.equip_serial_number,
        tm.module_mac_address,
        tm.server_name,
        tm.database_name AS tm_database_name,
        tm.company,
        tm.location_name,
        ROW_NUMBER() OVER (
            PARTITION BY tm.equip_serial_number
            ORDER BY tm.import_dt DESC, tm.insert_dt DESC
        ) AS tm_rn
    FROM LifecycleServicesReporting.dbo.Truck_Master AS tm
)

/* ---- Step 4: final result — tracker joined to Truck_Master ---- */
SELECT
    t.id,
    t.database_name,
    tm.company           AS customer_name,   -- customer from Truck_Master
    tm.location_name     AS location_name,   -- location from Truck_Master
    t.mfr_serial_number,
    t.source,
    t.eqpmnt_model_name,
    t.module_mac_address,
    t.module_model,
    t.last_comm_dt,
    t.device_name,
    t.device_type,
    t.firmware_version,
    t.boot_version,
    t.hardware_version,
    t.os_type,
    t.os_version,
    t.module_type,
    t.update_dt_utc,
    tm.server_name       AS tm_server_name,
    tm.tm_database_name
FROM tracker_deduped AS t
LEFT JOIN tm_deduped AS tm
       ON (
            tm.equip_serial_number = t.mfr_serial_number
            OR (t.mfr_serial_number IS NULL
                AND tm.module_mac_address = t.module_mac_address)
          )
      AND tm.tm_rn = 1
ORDER BY
    t.module_model,
    tm.company,
    tm.location_name,
    t.mfr_serial_number;
