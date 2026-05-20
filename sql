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
