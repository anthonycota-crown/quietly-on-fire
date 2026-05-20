/* ISR-5413 diagnostic — what HD6xxx module models actually exist in LCSR */
SELECT
    module_model,
    device_name,
    source,
    COUNT(*) AS row_count
FROM LifecycleServicesReporting.dbo.truck_firmware_tracker
WHERE module_model LIKE 'HD6%'
GROUP BY module_model, device_name, source
ORDER BY module_model, device_name, source;


/* ISR-5413 — HD6000 + HD6000c TDM modules NOT on target firmware 169044P10702001
   Source: LifecycleServicesReporting.dbo.truck_firmware_tracker (LCSR, SQL Server)
   Diagnostic-confirmed: HD6000 TDM ~21,901 rows + HD6000c TDM 96 rows (pre-filter) */

DECLARE @target_firmware varchar(50) = '169044P10702001';

SELECT
    tft.id,
    tft.database_name,
    tft.customer_name,
    tft.location_name,
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
    -- CRITERION 1: HD6000 family — both variants included so Anthony can filter HD6000c out himself
    tft.module_model IN ('HD6000', 'HD6000c')

    -- CRITERION 2: TDM modules only ("under this TDM version" — Tyler, Nov 6)
    AND tft.device_name = 'TDM'

    -- CRITERION 3: not on the target firmware build
    -- "under this version" still undefined by Tyler. Default = not-already-on-target. CONFIRM.
    AND (tft.firmware_version <> @target_firmware
         OR tft.firmware_version IS NULL)

    -- Active records only
    AND tft.current_flag = 1
    AND tft.deleted_flag != 1
ORDER BY tft.module_model, tft.source, tft.customer_name, tft.location_name, tft.mfr_serial_number;

