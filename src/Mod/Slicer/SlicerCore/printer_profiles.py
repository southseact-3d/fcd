"""
Printer and Material Profile Management for the Slicer Workbench.

Manages FDM and resin printer profiles, material definitions, and provides
profile import/export, persistence, and hierarchical settings merging.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import json
import os
import configparser
from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

# ---------------------------------------------------------------------------
# Type aliases
# ---------------------------------------------------------------------------

Size2D = Tuple[float, float]
Size3D = Tuple[float, float, float]
Range2 = Tuple[float, float]
FeedrateDict = Dict[str, float]
AccelerationDict = Dict[str, float]

# ---------------------------------------------------------------------------
# Abstract base
# ---------------------------------------------------------------------------


class Profile(ABC):
    """Base class for all printer / material profiles."""

    profile_type: str = "base"

    @abstractmethod
    def to_dict(self) -> Dict[str, Any]: ...

    @classmethod
    @abstractmethod
    def from_dict(cls, data: Dict[str, Any]) -> "Profile": ...


# ---------------------------------------------------------------------------
# FDM Printer Profile
# ---------------------------------------------------------------------------


@dataclass
class FDMProfile(Profile):
    """Hardware profile for an FDM/FFF printer."""

    profile_type: str = "fdm_printer"

    name: str = "Generic FDM"
    manufacturer: str = "Generic"
    model: str = "Unknown"
    bed_size: Size3D = (220.0, 220.0, 250.0)
    nozzle_diameter: float = 0.4
    heated_bed: bool = True
    heated_chamber: bool = False
    max_feedrate: FeedrateDict = field(
        default_factory=lambda: {"x": 300.0, "y": 300.0, "z": 10.0, "e": 60.0}
    )
    max_acceleration: AccelerationDict = field(
        default_factory=lambda: {"x": 1500.0, "y": 1500.0, "z": 100.0, "e": 5000.0}
    )
    firmware: str = "marlin"
    has_auto_bed_leveling: bool = False
    build_volume: Size3D = (220.0, 220.0, 250.0)

    def to_dict(self) -> Dict[str, Any]:
        return {
            "profile_type": self.profile_type,
            "name": self.name,
            "manufacturer": self.manufacturer,
            "model": self.model,
            "bed_size": list(self.bed_size),
            "nozzle_diameter": self.nozzle_diameter,
            "heated_bed": self.heated_bed,
            "heated_chamber": self.heated_chamber,
            "max_feedrate": dict(self.max_feedrate),
            "max_acceleration": dict(self.max_acceleration),
            "firmware": self.firmware,
            "has_auto_bed_leveling": self.has_auto_bed_leveling,
            "build_volume": list(self.build_volume),
        }

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "FDMProfile":
        return cls(
            name=data.get("name", "Unknown"),
            manufacturer=data.get("manufacturer", "Generic"),
            model=data.get("model", "Unknown"),
            bed_size=tuple(data.get("bed_size", (220.0, 220.0, 250.0))),
            nozzle_diameter=data.get("nozzle_diameter", 0.4),
            heated_bed=data.get("heated_bed", True),
            heated_chamber=data.get("heated_chamber", False),
            max_feedrate=data.get(
                "max_feedrate",
                {"x": 300.0, "y": 300.0, "z": 10.0, "e": 60.0},
            ),
            max_acceleration=data.get(
                "max_acceleration",
                {"x": 1500.0, "y": 1500.0, "z": 100.0, "e": 5000.0},
            ),
            firmware=data.get("firmware", "marlin"),
            has_auto_bed_leveling=data.get("has_auto_bed_leveling", False),
            build_volume=tuple(data.get("build_volume", (220.0, 220.0, 250.0))),
        )


# ---------------------------------------------------------------------------
# FDM Material Profile
# ---------------------------------------------------------------------------

VALID_FDM_MATERIAL_TYPES = (
    "PLA",
    "ABS",
    "PETG",
    "TPU",
    "Nylon",
    "PC",
    "ASA",
    "PVA",
)


@dataclass
class FDMaterialProfile(Profile):
    """Material / filament profile for FDM printers."""

    profile_type: str = "fdm_material"

    name: str = "Generic PLA"
    manufacturer: str = "Generic"
    material_type: str = "PLA"
    density: float = 1.24
    diameter: float = 1.75
    temperature_range: Range2 = (190.0, 230.0)
    bed_temperature_range: Range2 = (50.0, 70.0)
    print_speed_range: Range2 = (30.0, 80.0)
    fan_speed_range: Range2 = (0.0, 100.0)
    retract_distance: float = 5.0
    retract_speed: float = 45.0
    flow_rate: float = 100.0
    shrinkage_factor: float = 0.0

    def to_dict(self) -> Dict[str, Any]:
        return {
            "profile_type": self.profile_type,
            "name": self.name,
            "manufacturer": self.manufacturer,
            "material_type": self.material_type,
            "density": self.density,
            "diameter": self.diameter,
            "temperature_range": list(self.temperature_range),
            "bed_temperature_range": list(self.bed_temperature_range),
            "print_speed_range": list(self.print_speed_range),
            "fan_speed_range": list(self.fan_speed_range),
            "retract_distance": self.retract_distance,
            "retract_speed": self.retract_speed,
            "flow_rate": self.flow_rate,
            "shrinkage_factor": self.shrinkage_factor,
        }

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "FDMaterialProfile":
        return cls(
            name=data.get("name", "Unknown"),
            manufacturer=data.get("manufacturer", "Generic"),
            material_type=data.get("material_type", "PLA"),
            density=data.get("density", 1.24),
            diameter=data.get("diameter", 1.75),
            temperature_range=tuple(data.get("temperature_range", (190.0, 230.0))),
            bed_temperature_range=tuple(
                data.get("bed_temperature_range", (50.0, 70.0))
            ),
            print_speed_range=tuple(data.get("print_speed_range", (30.0, 80.0))),
            fan_speed_range=tuple(data.get("fan_speed_range", (0.0, 100.0))),
            retract_distance=data.get("retract_distance", 5.0),
            retract_speed=data.get("retract_speed", 45.0),
            flow_rate=data.get("flow_rate", 100.0),
            shrinkage_factor=data.get("shrinkage_factor", 0.0),
        )


# ---------------------------------------------------------------------------
# Resin Printer Profile
# ---------------------------------------------------------------------------

VALID_LIGHT_TYPES = ("LCD", "DLP", "LASER")


@dataclass
class ResinProfile(Profile):
    """Hardware profile for a resin (MSLA/DLP/Laser) printer."""

    profile_type: str = "resin_printer"

    name: str = "Generic Resin"
    manufacturer: str = "Generic"
    model: str = "Unknown"
    resolution: Tuple[int, int] = (2560, 1440)
    pixel_size: Size2D = (0.04725, 0.04725)
    build_area: Size2D = (121.0, 68.0)
    build_height: float = 150.0
    has_tilt: bool = False
    lift_height: float = 5.0
    lift_speed: float = 1.0
    retract_speed: float = 1.0
    light_type: str = "LCD"
    light_power: float = 200.0

    def to_dict(self) -> Dict[str, Any]:
        return {
            "profile_type": self.profile_type,
            "name": self.name,
            "manufacturer": self.manufacturer,
            "model": self.model,
            "resolution": list(self.resolution),
            "pixel_size": list(self.pixel_size),
            "build_area": list(self.build_area),
            "build_height": self.build_height,
            "has_tilt": self.has_tilt,
            "lift_height": self.lift_height,
            "lift_speed": self.lift_speed,
            "retract_speed": self.retract_speed,
            "light_type": self.light_type,
            "light_power": self.light_power,
        }

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ResinProfile":
        return cls(
            name=data.get("name", "Unknown"),
            manufacturer=data.get("manufacturer", "Generic"),
            model=data.get("model", "Unknown"),
            resolution=tuple(data.get("resolution", (2560, 1440))),
            pixel_size=tuple(data.get("pixel_size", (0.04725, 0.04725))),
            build_area=tuple(data.get("build_area", (121.0, 68.0))),
            build_height=data.get("build_height", 150.0),
            has_tilt=data.get("has_tilt", False),
            lift_height=data.get("lift_height", 5.0),
            lift_speed=data.get("lift_speed", 1.0),
            retract_speed=data.get("retract_speed", 1.0),
            light_type=data.get("light_type", "LCD"),
            light_power=data.get("light_power", 200.0),
        )


# ---------------------------------------------------------------------------
# Resin Material Profile
# ---------------------------------------------------------------------------


@dataclass
class ResinMaterialProfile(Profile):
    """Material / resin profile for SLA / MSLA / DLP printers."""

    profile_type: str = "resin_material"

    name: str = "Generic Grey Resin"
    manufacturer: str = "Generic"
    wavelength: int = 405
    exposure_time: float = 2.5
    bottom_exposure_time: float = 30.0
    bottom_layer_count: int = 5
    layer_height: float = 0.05
    light_off_delay: float = 1.0
    lift_height: float = 5.0
    lift_speed: float = 1.0
    retract_speed: float = 1.0
    shrinkage_factor: float = 1.0
    density: float = 1.1
    price_per_ml: float = 0.05
    color: str = "Grey"

    def to_dict(self) -> Dict[str, Any]:
        return {
            "profile_type": self.profile_type,
            "name": self.name,
            "manufacturer": self.manufacturer,
            "wavelength": self.wavelength,
            "exposure_time": self.exposure_time,
            "bottom_exposure_time": self.bottom_exposure_time,
            "bottom_layer_count": self.bottom_layer_count,
            "layer_height": self.layer_height,
            "light_off_delay": self.light_off_delay,
            "lift_height": self.lift_height,
            "lift_speed": self.lift_speed,
            "retract_speed": self.retract_speed,
            "shrinkage_factor": self.shrinkage_factor,
            "density": self.density,
            "price_per_ml": self.price_per_ml,
            "color": self.color,
        }

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ResinMaterialProfile":
        return cls(
            name=data.get("name", "Unknown"),
            manufacturer=data.get("manufacturer", "Generic"),
            wavelength=data.get("wavelength", 405),
            exposure_time=data.get("exposure_time", 2.5),
            bottom_exposure_time=data.get("bottom_exposure_time", 30.0),
            bottom_layer_count=data.get("bottom_layer_count", 5),
            layer_height=data.get("layer_height", 0.05),
            light_off_delay=data.get("light_off_delay", 1.0),
            lift_height=data.get("lift_height", 5.0),
            lift_speed=data.get("lift_speed", 1.0),
            retract_speed=data.get("retract_speed", 1.0),
            shrinkage_factor=data.get("shrinkage_factor", 1.0),
            density=data.get("density", 1.1),
            price_per_ml=data.get("price_per_ml", 0.05),
            color=data.get("color", "Grey"),
        )


# ---------------------------------------------------------------------------
# Profile type registry
# ---------------------------------------------------------------------------

_PROFILE_CLASSES: Dict[str, type] = {
    "fdm_printer": FDMProfile,
    "fdm_material": FDMaterialProfile,
    "resin_printer": ResinProfile,
    "resin_material": ResinMaterialProfile,
}


# ---------------------------------------------------------------------------
# Built-in default profiles
# ---------------------------------------------------------------------------

_BUILTIN_FDM_PRINTERS: List[Dict[str, Any]] = [
    {
        "name": "Creality Ender 3",
        "manufacturer": "Creality",
        "model": "Ender 3",
        "bed_size": [220.0, 220.0, 250.0],
        "nozzle_diameter": 0.4,
        "heated_bed": True,
        "heated_chamber": False,
        "max_feedrate": {"x": 500.0, "y": 500.0, "z": 10.0, "e": 60.0},
        "max_acceleration": {"x": 1000.0, "y": 1000.0, "z": 100.0, "e": 5000.0},
        "firmware": "marlin",
        "has_auto_bed_leveling": False,
        "build_volume": [220.0, 220.0, 250.0],
    },
    {
        "name": "Creality Ender 3 V2",
        "manufacturer": "Creality",
        "model": "Ender 3 V2",
        "bed_size": [220.0, 220.0, 250.0],
        "nozzle_diameter": 0.4,
        "heated_bed": True,
        "heated_chamber": False,
        "max_feedrate": {"x": 500.0, "y": 500.0, "z": 10.0, "e": 60.0},
        "max_acceleration": {"x": 1000.0, "y": 1000.0, "z": 100.0, "e": 5000.0},
        "firmware": "marlin",
        "has_auto_bed_leveling": False,
        "build_volume": [220.0, 220.0, 250.0],
    },
    {
        "name": "Prusa i3 MK3S+",
        "manufacturer": "Prusa Research",
        "model": "i3 MK3S+",
        "bed_size": [250.0, 210.0, 210.0],
        "nozzle_diameter": 0.4,
        "heated_bed": True,
        "heated_chamber": False,
        "max_feedrate": {"x": 200.0, "y": 200.0, "z": 12.0, "e": 120.0},
        "max_acceleration": {"x": 1000.0, "y": 1000.0, "z": 500.0, "e": 5000.0},
        "firmware": "marlin",
        "has_auto_bed_leveling": True,
        "build_volume": [250.0, 210.0, 210.0],
    },
    {
        "name": "Bambu Lab X1 Carbon",
        "manufacturer": "Bambu Lab",
        "model": "X1 Carbon",
        "bed_size": [256.0, 256.0, 256.0],
        "nozzle_diameter": 0.4,
        "heated_bed": True,
        "heated_chamber": True,
        "max_feedrate": {"x": 500.0, "y": 500.0, "z": 30.0, "e": 300.0},
        "max_acceleration": {"x": 20000.0, "y": 20000.0, "z": 5000.0, "e": 10000.0},
        "firmware": "griffin",
        "has_auto_bed_leveling": True,
        "build_volume": [256.0, 256.0, 256.0],
    },
    {
        "name": "Voron 2.4 300mm",
        "manufacturer": "Voron Design",
        "model": "2.4",
        "bed_size": [300.0, 300.0, 300.0],
        "nozzle_diameter": 0.4,
        "heated_bed": True,
        "heated_chamber": True,
        "max_feedrate": {"x": 400.0, "y": 400.0, "z": 25.0, "e": 60.0},
        "max_acceleration": {"x": 5000.0, "y": 5000.0, "z": 500.0, "e": 10000.0},
        "firmware": "repRap",
        "has_auto_bed_leveling": True,
        "build_volume": [300.0, 300.0, 300.0],
    },
]

_BUILTIN_FDM_MATERIALS: List[Dict[str, Any]] = [
    {
        "name": "Generic PLA",
        "manufacturer": "Generic",
        "material_type": "PLA",
        "density": 1.24,
        "diameter": 1.75,
        "temperature_range": [190.0, 230.0],
        "bed_temperature_range": [50.0, 70.0],
        "print_speed_range": [30.0, 80.0],
        "fan_speed_range": [50.0, 100.0],
        "retract_distance": 5.0,
        "retract_speed": 45.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 0.3,
    },
    {
        "name": "Generic ABS",
        "manufacturer": "Generic",
        "material_type": "ABS",
        "density": 1.04,
        "diameter": 1.75,
        "temperature_range": [220.0, 260.0],
        "bed_temperature_range": [90.0, 110.0],
        "print_speed_range": [30.0, 60.0],
        "fan_speed_range": [0.0, 30.0],
        "retract_distance": 5.0,
        "retract_speed": 45.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 0.8,
    },
    {
        "name": "Generic PETG",
        "manufacturer": "Generic",
        "material_type": "PETG",
        "density": 1.27,
        "diameter": 1.75,
        "temperature_range": [220.0, 250.0],
        "bed_temperature_range": [70.0, 90.0],
        "print_speed_range": [25.0, 60.0],
        "fan_speed_range": [20.0, 60.0],
        "retract_distance": 4.0,
        "retract_speed": 40.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 0.4,
    },
    {
        "name": "Generic TPU",
        "manufacturer": "Generic",
        "material_type": "TPU",
        "density": 1.21,
        "diameter": 1.75,
        "temperature_range": [210.0, 230.0],
        "bed_temperature_range": [40.0, 60.0],
        "print_speed_range": [10.0, 30.0],
        "fan_speed_range": [50.0, 100.0],
        "retract_distance": 1.5,
        "retract_speed": 25.0,
        "flow_rate": 105.0,
        "shrinkage_factor": 0.5,
    },
    {
        "name": "Generic Nylon",
        "manufacturer": "Generic",
        "material_type": "Nylon",
        "density": 1.14,
        "diameter": 1.75,
        "temperature_range": [240.0, 270.0],
        "bed_temperature_range": [70.0, 90.0],
        "print_speed_range": [25.0, 60.0],
        "fan_speed_range": [0.0, 50.0],
        "retract_distance": 5.0,
        "retract_speed": 40.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 1.5,
    },
    {
        "name": "Generic PC",
        "manufacturer": "Generic",
        "material_type": "PC",
        "density": 1.20,
        "diameter": 1.75,
        "temperature_range": [270.0, 310.0],
        "bed_temperature_range": [100.0, 120.0],
        "print_speed_range": [20.0, 50.0],
        "fan_speed_range": [0.0, 40.0],
        "retract_distance": 5.0,
        "retract_speed": 40.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 0.7,
    },
    {
        "name": "Generic ASA",
        "manufacturer": "Generic",
        "material_type": "ASA",
        "density": 1.07,
        "diameter": 1.75,
        "temperature_range": [230.0, 260.0],
        "bed_temperature_range": [90.0, 110.0],
        "print_speed_range": [25.0, 60.0],
        "fan_speed_range": [0.0, 30.0],
        "retract_distance": 5.0,
        "retract_speed": 45.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 0.7,
    },
    {
        "name": "Generic PVA",
        "manufacturer": "Generic",
        "material_type": "PVA",
        "density": 1.30,
        "diameter": 1.75,
        "temperature_range": [185.0, 205.0],
        "bed_temperature_range": [50.0, 70.0],
        "print_speed_range": [15.0, 40.0],
        "fan_speed_range": [50.0, 100.0],
        "retract_distance": 4.0,
        "retract_speed": 35.0,
        "flow_rate": 100.0,
        "shrinkage_factor": 0.2,
    },
]

_BUILTIN_RESIN_PRINTERS: List[Dict[str, Any]] = [
    {
        "name": "Anycubic Photon Mono X",
        "manufacturer": "Anycubic",
        "model": "Photon Mono X",
        "resolution": [3840, 2400],
        "pixel_size": [0.047, 0.047],
        "build_area": [192.0, 120.0],
        "build_height": 245.0,
        "has_tilt": False,
        "lift_height": 5.0,
        "lift_speed": 2.0,
        "retract_speed": 2.0,
        "light_type": "LCD",
        "light_power": 200.0,
    },
    {
        "name": "Elegoo Mars 3",
        "manufacturer": "Elegoo",
        "model": "Mars 3",
        "resolution": [4098, 2560],
        "pixel_size": [0.035, 0.035],
        "build_area": [143.0, 89.6],
        "build_height": 180.0,
        "has_tilt": False,
        "lift_height": 5.0,
        "lift_speed": 2.0,
        "retract_speed": 2.0,
        "light_type": "LCD",
        "light_power": 180.0,
    },
    {
        "name": "Formlabs Form 3",
        "manufacturer": "Formlabs",
        "model": "Form 3",
        "resolution": [2560, 1600],
        "pixel_size": [0.075, 0.075],
        "build_area": [195.0, 120.0],
        "build_height": 250.0,
        "has_tilt": True,
        "lift_height": 3.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "light_type": "LASER",
        "light_power": 250.0,
    },
    {
        "name": "Prusa SL1S",
        "manufacturer": "Prusa Research",
        "model": "SL1S",
        "resolution": [2560, 1620],
        "pixel_size": [0.0497, 0.0497],
        "build_area": [127.0, 80.5],
        "build_height": 180.0,
        "has_tilt": True,
        "lift_height": 2.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "light_type": "LCD",
        "light_power": 220.0,
    },
]

_BUILTIN_RESIN_MATERIALS: List[Dict[str, Any]] = [
    {
        "name": "Generic Grey Resin",
        "manufacturer": "Generic",
        "wavelength": 405,
        "exposure_time": 2.5,
        "bottom_exposure_time": 30.0,
        "bottom_layer_count": 5,
        "layer_height": 0.05,
        "light_off_delay": 1.0,
        "lift_height": 5.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "shrinkage_factor": 1.0,
        "density": 1.1,
        "price_per_ml": 0.05,
        "color": "Grey",
    },
    {
        "name": "Generic Clear Resin",
        "manufacturer": "Generic",
        "wavelength": 405,
        "exposure_time": 3.0,
        "bottom_exposure_time": 35.0,
        "bottom_layer_count": 5,
        "layer_height": 0.05,
        "light_off_delay": 1.0,
        "lift_height": 5.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "shrinkage_factor": 1.2,
        "density": 1.1,
        "price_per_ml": 0.06,
        "color": "Clear",
    },
    {
        "name": "Generic White Resin",
        "manufacturer": "Generic",
        "wavelength": 405,
        "exposure_time": 2.0,
        "bottom_exposure_time": 28.0,
        "bottom_layer_count": 5,
        "layer_height": 0.05,
        "light_off_delay": 1.0,
        "lift_height": 5.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "shrinkage_factor": 0.9,
        "density": 1.15,
        "price_per_ml": 0.05,
        "color": "White",
    },
    {
        "name": "Generic Tough Resin",
        "manufacturer": "Generic",
        "wavelength": 405,
        "exposure_time": 3.0,
        "bottom_exposure_time": 40.0,
        "bottom_layer_count": 6,
        "layer_height": 0.05,
        "light_off_delay": 1.5,
        "lift_height": 5.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "shrinkage_factor": 0.8,
        "density": 1.20,
        "price_per_ml": 0.08,
        "color": "Grey",
    },
    {
        "name": "Generic Water-Washable Resin",
        "manufacturer": "Generic",
        "wavelength": 405,
        "exposure_time": 2.0,
        "bottom_exposure_time": 25.0,
        "bottom_layer_count": 5,
        "layer_height": 0.05,
        "light_off_delay": 1.0,
        "lift_height": 5.0,
        "lift_speed": 1.0,
        "retract_speed": 1.0,
        "shrinkage_factor": 1.0,
        "density": 1.10,
        "price_per_ml": 0.06,
        "color": "Grey",
    },
]

# ---------------------------------------------------------------------------
# Default FDM settings (used as base for merging)
# ---------------------------------------------------------------------------

_DEFAULT_FDM_SETTINGS: Dict[str, Any] = {
    "layer_height": 0.2,
    "first_layer_height": 0.3,
    "perimeters": 3,
    "top_solid_layers": 4,
    "bottom_solid_layers": 4,
    "fill_density": 20.0,
    "fill_pattern": "grid",
    "print_speed": 60.0,
    "first_layer_speed": 20.0,
    "travel_speed": 150.0,
    "retraction_distance": 5.0,
    "retraction_speed": 45.0,
    "nozzle_diameter": 0.4,
    "filament_diameter": 1.75,
    "extrusion_multiplier": 1.0,
    "bed_temperature": 60.0,
    "temperature": 210.0,
    "fan_speed": 100.0,
    "first_layer_fan_speed": 0.0,
    "support_enabled": False,
    "support_angle": 45.0,
    "support_density": 15.0,
    "brim_width": 0.0,
    "raft_layers": 0,
    "skirt_lines": 1,
    "skirts": 1,
}

_DEFAULT_RESIN_SETTINGS: Dict[str, Any] = {
    "layer_height": 0.05,
    "bottom_layer_count": 5,
    "exposure_time": 2.5,
    "bottom_exposure_time": 30.0,
    "light_off_delay": 1.0,
    "lift_height": 5.0,
    "lift_speed": 1.0,
    "retract_speed": 1.0,
    "anti_aliasing": True,
    "anti_aliasing_level": 4,
    "support_enabled": True,
    "support_density": 0.7,
    "support_point_distance": 0.5,
    "support_tip_diameter": 0.4,
    "support_base_diameter": 1.0,
    "support_base_height": 1.0,
    "raft_enabled": True,
    "raft_margin": 3.0,
}


# ---------------------------------------------------------------------------
# Settings Merger
# ---------------------------------------------------------------------------


class SettingsMerger:
    """Merge printer, material, and user settings with priority.

    Priority (highest to lowest):
        1. custom_settings  (user overrides)
        2. material_profile (material-specific values)
        3. printer_profile  (printer hardware defaults)
        4. _DEFAULT_*       (hard-coded fallbacks)
    """

    @staticmethod
    def _resolve_material(material: Profile) -> Dict[str, Any]:
        if isinstance(material, FDMaterialProfile):
            return {
                "temperature": max(material.temperature_range),
                "bed_temperature": max(material.bed_temperature_range),
                "print_speed": (
                    material.print_speed_range[0] + material.print_speed_range[1]
                )
                / 2.0,
                "fan_speed": (material.fan_speed_range[0] + material.fan_speed_range[1])
                / 2.0,
                "retraction_distance": material.retract_distance,
                "retraction_speed": material.retract_speed,
                "extrusion_multiplier": material.flow_rate / 100.0,
                "filament_diameter": material.diameter,
                "nozzle_diameter": 0.4,
            }
        if isinstance(material, ResinMaterialProfile):
            return {
                "layer_height": material.layer_height,
                "exposure_time": material.exposure_time,
                "bottom_exposure_time": material.bottom_exposure_time,
                "bottom_layer_count": material.bottom_layer_count,
                "light_off_delay": material.light_off_delay,
                "lift_height": material.lift_height,
                "lift_speed": material.lift_speed,
                "retract_speed": material.retract_speed,
            }
        return {}

    @staticmethod
    def _resolve_printer(printer: Profile) -> Dict[str, Any]:
        if isinstance(printer, FDMProfile):
            return {
                "nozzle_diameter": printer.nozzle_diameter,
            }
        if isinstance(printer, ResinProfile):
            return {
                "printer_width_mm": printer.build_area[0],
                "printer_height_mm": printer.build_area[1],
                "printer_resolution_x": printer.resolution[0],
                "printer_resolution_y": printer.resolution[1],
                "lift_height": printer.lift_height,
                "lift_speed": printer.lift_speed,
                "retract_speed": printer.retract_speed,
            }
        return {}

    @classmethod
    def merge(
        cls,
        printer_profile: Optional[Profile] = None,
        material_profile: Optional[Profile] = None,
        custom_settings: Optional[Dict[str, Any]] = None,
    ) -> Dict[str, Any]:
        """Return a single merged settings dictionary.

        Parameters
        ----------
        printer_profile : Profile, optional
            The printer hardware profile.
        material_profile : Profile, optional
            The material / filament profile.
        custom_settings : dict, optional
            User-supplied overrides that win over everything.

        Returns
        -------
        dict
            Flat dictionary of slicer settings.
        """
        is_resin = isinstance(printer_profile, ResinProfile) or isinstance(
            material_profile, ResinMaterialProfile
        )
        base = (
            dict(_DEFAULT_RESIN_SETTINGS) if is_resin else dict(_DEFAULT_FDM_SETTINGS)
        )

        if printer_profile is not None:
            base.update(cls._resolve_printer(printer_profile))
        if material_profile is not None:
            base.update(cls._resolve_material(material_profile))
        if custom_settings:
            base.update(custom_settings)

        return base


# ---------------------------------------------------------------------------
# Profile Manager
# ---------------------------------------------------------------------------


class ProfileManager:
    """Load, save, delete, import, and export slicer profiles.

    Profiles are stored as individual JSON files under a ``Profiles/``
    directory that lives next to this module by default.

    Directory layout::

        Profiles/
            fdm_printer/
                Creality Ender 3.json
                Prusa i3 MK3S+.json
            fdm_material/
                Generic PLA.json
            resin_printer/
                Anycubic Photon Mono X.json
            resin_material/
                Generic Grey Resin.json
    """

    _DIR_NAMES: Dict[str, str] = {
        "fdm_printer": "fdm_printer",
        "fdm_material": "fdm_material",
        "resin_printer": "resin_printer",
        "resin_material": "resin_material",
    }

    def __init__(self, profile_dir: Optional[str | Path] = None) -> None:
        if profile_dir is None:
            profile_dir = Path(__file__).resolve().parent / "Profiles"
        self._root = Path(profile_dir)
        self._root.mkdir(parents=True, exist_ok=True)
        for subdir in self._DIR_NAMES.values():
            (self._root / subdir).mkdir(parents=True, exist_ok=True)

    # -- helpers -----------------------------------------------------------

    def _dir_for(self, profile_type: str) -> Path:
        name = self._DIR_NAMES.get(profile_type)
        if name is None:
            raise ValueError(f"Unknown profile type: {profile_type!r}")
        return self._root / name

    def _path_for(self, name: str, profile_type: str) -> Path:
        safe = name.replace("/", "_").replace("\\", "_")
        return self._dir_for(profile_type) / f"{safe}.json"

    @staticmethod
    def _profile_class_for(profile_type: str) -> type:
        cls = _PROFILE_CLASSES.get(profile_type)
        if cls is None:
            raise ValueError(f"Unknown profile type: {profile_type!r}")
        return cls

    # -- public API --------------------------------------------------------

    def get_fdm_printers(self) -> List[FDMProfile]:
        """Return all FDM printer profiles (built-in + user-saved)."""
        profiles = [FDMProfile.from_dict(d) for d in _BUILTIN_FDM_PRINTERS]
        profiles.extend(self._load_from_dir("fdm_printer", FDMProfile))
        return profiles

    def get_fdm_materials(self) -> List[FDMaterialProfile]:
        """Return all FDM material profiles (built-in + user-saved)."""
        profiles = [FDMaterialProfile.from_dict(d) for d in _BUILTIN_FDM_MATERIALS]
        profiles.extend(self._load_from_dir("fdm_material", FDMaterialProfile))
        return profiles

    def get_resin_printers(self) -> List[ResinProfile]:
        """Return all resin printer profiles (built-in + user-saved)."""
        profiles = [ResinProfile.from_dict(d) for d in _BUILTIN_RESIN_PRINTERS]
        profiles.extend(self._load_from_dir("resin_printer", ResinProfile))
        return profiles

    def get_resin_materials(self) -> List[ResinMaterialProfile]:
        """Return all resin material profiles (built-in + user-saved)."""
        profiles = [ResinMaterialProfile.from_dict(d) for d in _BUILTIN_RESIN_MATERIALS]
        profiles.extend(self._load_from_dir("resin_material", ResinMaterialProfile))
        return profiles

    def load_printer(self, name: str, profile_type: str) -> Profile:
        """Load a single profile by *name* and *profile_type*.

        Raises ``FileNotFoundError`` if no matching file exists on disk.
        Built-in profiles are also searchable.
        """
        cls = self._profile_class_for(profile_type)

        if profile_type in ("fdm_printer", "fdm_material"):
            builtins = (
                _BUILTIN_FDM_PRINTERS
                if profile_type == "fdm_printer"
                else _BUILTIN_FDM_MATERIALS
            )
            for d in builtins:
                if d.get("name") == name:
                    return cls.from_dict(d)

        if profile_type in ("resin_printer", "resin_material"):
            builtins = (
                _BUILTIN_RESIN_PRINTERS
                if profile_type == "resin_printer"
                else _BUILTIN_RESIN_MATERIALS
            )
            for d in builtins:
                if d.get("name") == name:
                    return cls.from_dict(d)

        path = self._path_for(name, profile_type)
        if not path.exists():
            raise FileNotFoundError(
                f"No profile named {name!r} of type {profile_type!r}"
            )
        with open(path, "r", encoding="utf-8") as fh:
            data = json.load(fh)
        return cls.from_dict(data)

    def save_printer(self, profile: Profile) -> None:
        """Persist *profile* to disk as JSON."""
        if not isinstance(profile, Profile):
            raise TypeError(f"Expected Profile instance, got {type(profile).__name__}")
        path = self._path_for(profile.name, profile.profile_type)
        with open(path, "w", encoding="utf-8") as fh:
            json.dump(profile.to_dict(), fh, indent=2)

    def delete_printer(self, name: str, profile_type: str) -> bool:
        """Delete a user-saved profile.  Returns True if deleted, False otherwise.

        Built-in profiles cannot be deleted.
        """
        if self._is_builtin(name, profile_type):
            return False
        path = self._path_for(name, profile_type)
        if path.exists():
            path.unlink()
            return True
        return False

    def import_profile(self, filepath: str | Path) -> Profile:
        """Import a profile from a JSON or INI file and return it.

        The profile type is read from the JSON ``profile_type`` key, or
        inferred for INI files based on section names.
        """
        p = Path(filepath)
        if not p.exists():
            raise FileNotFoundError(f"File not found: {p}")

        if p.suffix.lower() == ".json":
            return self._import_json(p)
        if p.suffix.lower() in (".ini", ".cfg"):
            return self._import_ini(p)
        raise ValueError(f"Unsupported file format: {p.suffix!r}")

    def export_profile(self, profile: Profile, filepath: str | Path) -> None:
        """Export *profile* to a JSON file at *filepath*."""
        p = Path(filepath)
        p.parent.mkdir(parents=True, exist_ok=True)
        with open(p, "w", encoding="utf-8") as fh:
            json.dump(profile.to_dict(), fh, indent=2)

    def get_default_profile(self, printer_type: str) -> Profile:
        """Return a sensible default profile for *printer_type*.

        ``printer_type`` must be one of the four ``profile_type`` strings.
        """
        cls = self._profile_class_for(printer_type)
        if cls is FDMProfile:
            return FDMProfile()
        if cls is FDMaterialProfile:
            return FDMaterialProfile()
        if cls is ResinProfile:
            return ResinProfile()
        if cls is ResinMaterialProfile:
            return ResinMaterialProfile()
        raise ValueError(f"No default for {printer_type!r}")

    # -- internal helpers --------------------------------------------------

    def _load_from_dir(self, profile_type: str, cls: type) -> List[Profile]:
        d = self._dir_for(profile_type)
        profiles: List[Profile] = []
        if not d.exists():
            return profiles
        for fp in sorted(d.glob("*.json")):
            try:
                with open(fp, "r", encoding="utf-8") as fh:
                    data = json.load(fh)
                profiles.append(cls.from_dict(data))
            except (json.JSONDecodeError, KeyError):
                continue
        return profiles

    def _import_json(self, path: Path) -> Profile:
        with open(path, "r", encoding="utf-8") as fh:
            data = json.load(fh)
        ptype = data.get("profile_type")
        if ptype is None:
            ptype = self._guess_type_from_keys(data)
        cls = self._profile_class_for(ptype)
        return cls.from_dict(data)

    def _import_ini(self, path: Path) -> Profile:
        cfg = configparser.ConfigParser()
        cfg.read(str(path), encoding="utf-8")

        section_names = cfg.sections()
        if not section_names:
            raise ValueError("INI file has no sections")

        first = section_names[0].lower()
        data: Dict[str, Any] = {}

        if "fdm" in first or "printer" in first:
            data["profile_type"] = "fdm_printer"
            data.update(self._ini_to_dict(cfg))
        elif "material" in first or "filament" in first:
            data["profile_type"] = "fdm_material"
            data.update(self._ini_to_dict(cfg))
        elif "resin" in first:
            data["profile_type"] = (
                "resin_material" if "material" in first else "resin_printer"
            )
            data.update(self._ini_to_dict(cfg))
        else:
            data["profile_type"] = "fdm_printer"
            data.update(self._ini_to_dict(cfg))

        cls = self._profile_class_for(data["profile_type"])
        return cls.from_dict(data)

    @staticmethod
    def _ini_to_dict(cfg: configparser.ConfigParser) -> Dict[str, Any]:
        out: Dict[str, Any] = {}
        for section in cfg.sections():
            for key, val in cfg.items(section):
                key = key.strip()
                try:
                    out[key] = json.loads(val)
                except (json.JSONDecodeError, TypeError):
                    out[key] = val
        return out

    @staticmethod
    def _guess_type_from_keys(data: Dict[str, Any]) -> str:
        if "nozzle_diameter" in data or "heated_bed" in data:
            return "fdm_printer"
        if "material_type" in data or "temperature_range" in data:
            return "fdm_material"
        if "resolution" in data and "pixel_size" in data:
            return "resin_printer"
        if "exposure_time" in data and "bottom_exposure_time" in data:
            return "resin_material"
        return "fdm_printer"

    def _is_builtin(self, name: str, profile_type: str) -> bool:
        if profile_type == "fdm_printer":
            return any(d["name"] == name for d in _BUILTIN_FDM_PRINTERS)
        if profile_type == "fdm_material":
            return any(d["name"] == name for d in _BUILTIN_FDM_MATERIALS)
        if profile_type == "resin_printer":
            return any(d["name"] == name for d in _BUILTIN_RESIN_PRINTERS)
        if profile_type == "resin_material":
            return any(d["name"] == name for d in _BUILTIN_RESIN_MATERIALS)
        return False
