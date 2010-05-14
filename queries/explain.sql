SET SEARCH_PATH = test_data, test_view;
/*
 * moderately complex query
 */
EXPLAIN (FORMAT XML)
SELECT *
FROM
    proband
    JOIN proband__sample USING(proband_id)
    JOIN sample USING (sample_id)

    JOIN information_generic_allowance USING (proband_id)
    JOIN information_genetic_allowance USING (proband_id)
    JOIN information_patient_diagnosis diag_1 USING (proband_id)
    JOIN information_patient_diagnosis diag_2 USING (proband_id)
    JOIN information_patient_placeholder patient_histology USING (proband_id)

    JOIN information_sample_placeholder sample_rna_experiment_1_status USING (sample_id)
    JOIN information_sample_placeholder sample_rna_experiment_1_data USING (sample_id)

    JOIN information_sample_placeholder sample_rna_experiment_2_status USING (sample_id)
    JOIN information_sample_placeholder sample_rna_experiment_2_data USING (sample_id)

    JOIN information_sample_placeholder sample_rna_experiment_3_status USING (sample_id)
    JOIN information_sample_placeholder sample_rna_experiment_3_data USING (sample_id)

WHERE TRUE
    AND information_generic_allowance.generic_allowance_granted = true
    AND information_genetic_allowance.information_6_granted = true

    AND diag_1.diagnosis_icd10 = 1343 /*some icd code*/
    AND diag_2.diagnosis_icd10 = 1344 /*another icd code*/

    AND (
        SELECT
 	    available_information_nr / available_information_nr
    	FROM
	    information_set_completition_status
	WHERE
	    information_set_completition_status.information_set_instance_id = patient_histology.information_set_instance_id
    ) > 0.8


    AND (
        SELECT
 	    available_information_nr / available_information_nr
    	FROM
	    information_set_completition_status
	WHERE
	    information_set_completition_status.information_set_instance_id = diag_1.information_set_instance_id
    ) > 0.5


    AND (
        SELECT
 	    available_information_nr / available_information_nr
    	FROM
	    information_set_completition_status
	WHERE
	    information_set_completition_status.information_set_instance_id = diag_2.information_set_instance_id
    ) > 0.5;
