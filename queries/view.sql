DROP SCHEMA IF EXISTS test_view CASCADE;
CREATE SCHEMA test_view;
SET SEARCH_PATH = test_view, test_data;


CREATE OR REPLACE VIEW information_set_completition_status AS
SELECT
    information_set.information_set_id,
    information_set_instance.information_set_instance_id,
    COUNT(information.information_id) AS possible_information_nr,
    COUNT(information_instance.information_id) AS available_information_nr
FROM
    information_set_instance
    JOIN information_set USING(information_set_id)
    JOIN information USING(information_set_id)
    LEFT JOIN information_instance USING(information_id)
GROUP BY
    information_set.information_set_id,
    information_set_instance.information_set_instance_id;


CREATE OR REPLACE VIEW information_generic_allowance AS
SELECT
    information_set_instance__proband.proband_id

    ,information_set.information_set_id
    ,information_set_instance.information_set_instance_id

    ,generic_allowance.granted AS generic_allowance_granted
    ,anonymized_use_noncommercial_research.granted AS anonymized_use_noncommercial_research_granted
    ,anonymized_use_commercial_research.granted AS anonymized_use_commercial_research_granted
    ,contact_in_noncommercial_research.granted AS contact_in_noncommercial_research_granted
    ,contact_in_commercial_research.granted AS contact_in_commercial_research_granted
    ,information_6.granted AS information_6_granted
    ,information_7.granted AS information_7_granted
    ,information_8.granted AS information_8_granted
    ,information_9.granted AS information_9_granted
    ,information_10.granted AS information_10_granted
    ,information_11.granted AS information_11_granted

FROM
    information_set_instance__proband
    JOIN information_set_instance USING (information_set_instance_id)
    JOIN information_set USING (information_set_id)

    LEFT JOIN (
    	SELECT
            information.information_set_id,
            information_about_allowance.granted
   	 FROM
   	     information
   	     JOIN information_instance USING (information_id)
   	     JOIN information_about_allowance USING (information_instance_id)
   	 WHERE true
   	     AND information.name = 'generic_allowance'
    ) AS generic_allowance
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'anonymized_use_noncommercial_research'
    ) AS anonymized_use_noncommercial_research
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'anonymized_use_commercial_research'
    ) AS anonymized_use_commercial_research
    USING (information_set_id)
    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'contact_in_noncommercial_research'
    ) AS contact_in_noncommercial_research
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'contact_in_commercial_research'
    ) AS contact_in_commercial_research
    USING (information_set_id)

    /*
     * All further joins have pointless names to ease the writing
     */
    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_5'
    ) AS information_6
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_6'
    ) AS information_7
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_7'
    ) AS information_8
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_8'
    ) AS information_9
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_9'
    ) AS information_10
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_10'
    ) AS information_11
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
    	    information.information_set_id,
    	    information_about_allowance.granted
    	FROM
    	    information
    	    JOIN information_instance USING (information_id)
    	    JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
    	    AND information.name = 'information_11'
    ) AS information_12
    USING (information_set_id)
WHERE TRUE
    AND information_set.name = 'generic_allowance_v3';


CREATE OR REPLACE VIEW information_genetic_allowance AS
SELECT
    information_set_instance__proband.proband_id

    ,information_set.information_set_id
    ,information_set_instance.information_set_instance_id

    ,notification_incurable_illness_found.granted AS notification_incurable_illness_found_granted
    ,notification_curable_illness_found.granted AS notification_curable_illness_found_granted
    ,notification_inheritable_illness_found.granted AS notification_inheritable_illness_found_granted
    ,use_of_genetic_data_if_ill_internal.granted AS use_of_genetic_data_if_ill_internal_granted
    ,use_of_genetic_data_if_ill_external.granted AS use_of_genetic_data_if_ill_external_granted
    ,information_6.granted AS information_6_granted
    ,information_7.granted AS information_7_granted
    ,information_8.granted AS information_8_granted
    ,information_9.granted AS information_9_granted
    ,information_10.granted AS information_10_granted
    ,information_11.granted AS information_11_granted
    ,information_12.granted AS information_12_granted
    ,information_13.granted AS information_13_granted
    ,information_14.granted AS information_14_granted
    ,information_15.granted AS information_15_granted
    ,information_16.granted AS information_16_granted
    ,information_17.granted AS information_17_granted
    ,information_18.granted AS information_18_granted
    ,information_19.granted AS information_19_granted
    ,information_20.granted AS information_20_granted
    ,information_21.granted AS information_21_granted


FROM
    information_set_instance__proband
    JOIN information_set_instance USING (information_set_instance_id)
    JOIN information_set USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'notification_incurable_illness_found'
    ) AS notification_incurable_illness_found
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'notification_curable_illness_found'
    ) AS notification_curable_illness_found
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'notification_inheritable_illness_found'
    ) AS notification_inheritable_illness_found
    USING (information_set_id)
    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'use_of_genetic_data_if_ill_internal'
    ) AS use_of_genetic_data_if_ill_internal
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'use_of_genetic_data_if_ill_external'
    ) AS use_of_genetic_data_if_ill_external
    USING (information_set_id)

    /*
     * All further joins have pointless names to ease the writing
     */
    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_6'
    ) AS information_6
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_7'
    ) AS information_7
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_8'
    ) AS information_8
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_9'
    ) AS information_9
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_10'
    ) AS information_10
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_11'
    ) AS information_11
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_12'
    ) AS information_12

    USING (information_set_id)
    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_13'
    ) AS information_13
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_14'
    ) AS information_14
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_15'
    ) AS information_15
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_16'
    ) AS information_16
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_17'
    ) AS information_17
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_18'
    ) AS information_18
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_19'
    ) AS information_19
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_20'
    ) AS information_20
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_allowance.granted
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_allowance USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_21'
    ) AS information_21
    USING (information_set_id)
WHERE TRUE
    AND information_set.name = 'genetic_allowance_v3';







CREATE OR REPLACE VIEW information_patient_diagnosis AS
SELECT
    information_set_instance__proband.proband_id

    ,information_set.information_set_id
    ,information_set_instance.information_set_instance_id

    ,diagnosis_icd10.data AS diagnosis_icd10
FROM
    information_set_instance__proband
    JOIN information_set_instance USING (information_set_instance_id)
    JOIN information_set USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_icd10_classification.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_icd10_classification USING (information_instance_id)
    	WHERE true
            AND information.name = 'diagnosis'
    ) AS diagnosis_icd10
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_time.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_time USING (information_instance_id)
    	WHERE true
            AND information.name = 'diagnosis_date'
    ) AS diagnosis_date
    USING (information_set_id)


    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_clinic.clinic_id
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_clinic USING (information_instance_id)
    	WHERE true
            AND information.name = 'diagnosing_clinic'
    ) AS diagnosis_clinic
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_personel.name
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_personel USING (information_instance_id)
    	WHERE true
            AND information.name = 'diagnosing_personel'
    ) AS diagnosis_personel
    USING (information_set_id)


    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_1'
    ) AS information_1
    USING (information_set_id)


    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_2'
    ) AS information_2
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_3'
    ) AS information_3
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_4'
    ) AS information_4
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_5'
    ) AS information_5
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_6'
    ) AS information_6
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_7'
    ) AS information_7
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_8'
    ) AS information_8
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_9'
    ) AS information_9
    USING (information_set_id)


    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_10'
    ) AS information_10
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_11'
    ) AS information_11
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_12'
    ) AS information_12
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_13'
    ) AS information_13
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_14'
    ) AS information_14
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_15'
    ) AS information_15
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_16'
    ) AS information_16
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_17'
    ) AS information_17
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_18'
    ) AS information_18
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_19'
    ) AS information_19
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_20'
    ) AS information_20
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_21'
    ) AS information_21
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_22'
    ) AS information_22
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_23'
    ) AS information_23
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_24'
    ) AS information_24
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_25'
    ) AS information_25
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_26'
    ) AS information_26
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_27'
    ) AS information_27
    USING (information_set_id)


WHERE TRUE
    AND information_set.name = 'patient_diagnosis_plain';



CREATE OR REPLACE VIEW information_patient_placeholder AS
SELECT
    information_set_instance__proband.proband_id

    ,information_set.information_set_id
    ,information_set_instance.information_set_instance_id

    ,information_1.data AS information_1_data
    ,information_2.data AS information_2_data
    ,information_3.data AS information_3_data
    ,information_4.data AS information_4_data
    ,information_5.data AS information_5_data
    ,information_6.data AS information_6_data
    ,information_7.data AS information_7_data
    ,information_8.data AS information_8_data
    ,information_9.data AS information_9_data
    ,information_10.data AS information_10_data
    ,information_11.data AS information_11_data
    ,information_12.data AS information_12_data
    ,information_13.data AS information_13_data
    ,information_14.data AS information_14_data
    ,information_15.data AS information_15_data
    ,information_16.data AS information_16_data
    ,information_17.data AS information_17_data
    ,information_18.data AS information_18_data
    ,information_19.data AS information_19_data
    ,information_20.data AS information_20_data
    ,information_21.data AS information_21_data
    ,information_22.data AS information_22_data
    ,information_23.data AS information_23_data
    ,information_24.data AS information_24_data
    ,information_25.data AS information_25_data
    ,information_26.data AS information_26_data
    ,information_27.data AS information_27_data
FROM
    information_set_instance__proband
    JOIN information_set_instance USING (information_set_instance_id)
    JOIN information_set USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_1'
    ) AS information_1
    USING (information_set_id)


    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_2'
    ) AS information_2
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_3'
    ) AS information_3
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_4'
    ) AS information_4
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_5'
    ) AS information_5
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_6'
    ) AS information_6
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_7'
    ) AS information_7
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_8'
    ) AS information_8
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_9'
    ) AS information_9
    USING (information_set_id)


    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_10'
    ) AS information_10
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_11'
    ) AS information_11
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_12'
    ) AS information_12
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_13'
    ) AS information_13
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_14'
    ) AS information_14
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_15'
    ) AS information_15
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_16'
    ) AS information_16
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_17'
    ) AS information_17
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_18'
    ) AS information_18
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_19'
    ) AS information_19
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_20'
    ) AS information_20
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_21'
    ) AS information_21
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_22'
    ) AS information_22
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_23'
    ) AS information_23
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_24'
    ) AS information_24
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_25'
    ) AS information_25
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_26'
    ) AS information_26
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_27'
    ) AS information_27
    USING (information_set_id)


WHERE TRUE
    AND information_set.name = 'patient_placeholder';



CREATE OR REPLACE VIEW information_sample_placeholder AS
SELECT
    information_set_instance__sample.sample_id

    ,information_set.information_set_id
    ,information_set_instance.information_set_instance_id

    ,information_1.data AS information_1_data
    ,information_2.data AS information_2_data
    ,information_3.data AS information_3_data
    ,information_4.data AS information_4_data
    ,information_5.data AS information_5_data
    ,information_6.data AS information_6_data
    ,information_7.data AS information_7_data
    ,information_8.data AS information_8_data
    ,information_9.data AS information_9_data
    ,information_10.data AS information_10_data
    ,information_11.data AS information_11_data
    ,information_12.data AS information_12_data
    ,information_13.data AS information_13_data
    ,information_14.data AS information_14_data
    ,information_15.data AS information_15_data
    ,information_16.data AS information_16_data
    ,information_17.data AS information_17_data
    ,information_18.data AS information_18_data
    ,information_19.data AS information_19_data
    ,information_20.data AS information_20_data
    ,information_21.data AS information_21_data
    ,information_22.data AS information_22_data
    ,information_23.data AS information_23_data
    ,information_24.data AS information_24_data
    ,information_25.data AS information_25_data
    ,information_26.data AS information_26_data
    ,information_27.data AS information_27_data

FROM
    information_set_instance__sample
    JOIN information_set_instance USING (information_set_instance_id)
    JOIN information_set USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_1'
    ) AS information_1
    USING (information_set_id)


    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_2'
    ) AS information_2
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_3'
    ) AS information_3
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_4'
    ) AS information_4
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_5'
    ) AS information_5
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_6'
    ) AS information_6
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_7'
    ) AS information_7
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_8'
    ) AS information_8
    USING (information_set_id)

    JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_9'
    ) AS information_9
    USING (information_set_id)


    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_10'
    ) AS information_10
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_11'
    ) AS information_11
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_12'
    ) AS information_12
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_13'
    ) AS information_13
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_14'
    ) AS information_14
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_15'
    ) AS information_15
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_16'
    ) AS information_16
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_17'
    ) AS information_17
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_18'
    ) AS information_18
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_19'
    ) AS information_19
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_20'
    ) AS information_20
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_21'
    ) AS information_21
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_22'
    ) AS information_22
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_23'
    ) AS information_23
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_24'
    ) AS information_24
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_25'
    ) AS information_25
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_26'
    ) AS information_26
    USING (information_set_id)

    LEFT JOIN (
    	SELECT
	    information.information_set_id,
       	    information_about_placeholder.data
        FROM
            information
            JOIN information_instance USING (information_id)
            JOIN information_about_placeholder USING (information_instance_id)
    	WHERE true
            AND information.name = 'information_27'
    ) AS information_27
    USING (information_set_id)


WHERE TRUE
    AND information_set.name = 'sample_placeholder';
