/* This code is part of the GenSync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on October, 2020.
 */

#include <GenSync/Aux/Sketches.h>

#include <memory>

const string Sketches::PRINT_KEY = "Sketches";
const uint Sketches::HLL_LOG_K = 14;
const uint Sketches::FI_LOG_MAX_SIZE = 10;

Sketches::Sketches(std::initializer_list<Types> sketches) {
    for (auto st : sketches)
        switch (st) {
        case Types::CARDINALITY:
            cardinality.value = std::make_unique<int>(0);
            cardinality.initiated = true;
            break;
        case Types::UNIQUE_ELEM:
            uniqueElem.value = std::make_unique<datasketches::hll_sketch_alloc<>>(HLL_LOG_K);
            uniqueElem.initiated = true;
            break;
        case Types::HEAVY_HITTERS:
            heavyHitters.value = std::make_unique<datasketches::frequent_items_sketch<unsigned long>>(FI_LOG_MAX_SIZE);
            heavyHitters.initiated = true;
            break;
        }
}

Sketches::Sketches() {
    cardinality.value = std::make_unique<int>(0);
    cardinality.initiated = true;

    uniqueElem.value = std::make_unique<datasketches::hll_sketch_alloc<>>(HLL_LOG_K);
    uniqueElem.initiated = true;

    heavyHitters.value = std::make_unique<datasketches::frequent_items_sketch<unsigned long>>(FI_LOG_MAX_SIZE);
    heavyHitters.initiated = true;
}

void Sketches::inc(const std::shared_ptr<DataObject>& elem) const {
    if (cardinality.initiated)
        (*cardinality.value)++;

    if (uniqueElem.initiated)
        uniqueElem.value->update(to_uint(elem->to_ZZ()));

    if (heavyHitters.initiated)
        heavyHitters.value->update(to_uint(elem->to_ZZ()));
}

Sketches::Values Sketches::get() const {
    Values ret;

    if (cardinality.initiated)
        ret.cardinality = *(cardinality.value);

    if (uniqueElem.initiated)
        ret.uniqueElem = uniqueElem.value->get_estimate();

    if (uniqueElem.initiated)
        ret.heavyHitters = heavyHitters.value->get_frequent_items(datasketches::NO_FALSE_POSITIVES).size();

    return ret;
}
