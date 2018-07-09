#include "pch.h"

#include <gamekid/cpu/impl/rotation.h>
#include <gamekid.tests/test_operand.h>
#include <gamekid/system.h>


using gamekid::cpu::impl::rotation;

TEST(ROTATION, RL_TURN_OFF_CARRY) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    cpu.F.carry(true);
    test_operand<byte> test_val(1);

    
    // Act
    rotation::rl_operation(cpu, test_val);

    // Assert
    // 1 rl = 2
    ASSERT_EQ(test_val.value, 2);
    ASSERT_EQ(cpu.F.carry(), false);
}

TEST(ROTATION, RL_TURN_CARRY_ON) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(128);

    // Act
    rotation::rl_operation(cpu, test_val);

    // Assert
    // 128 rl = 1
    ASSERT_EQ(test_val.value, 1);
    ASSERT_EQ(cpu.F.carry(), true);
}

TEST(ROTATION, RLC_TURN_CARRY_ON) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(1 << 7);

    // Act
    rotation::rlc_operation(cpu, test_val);

    // Assert
    // 128 rlc[carry = 0] = 0
    ASSERT_EQ(test_val.value, 0);
    ASSERT_EQ(cpu.F.carry(), true);
}

TEST(ROTATION, RLC_CARRY_ON){
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    cpu.F.carry(true);
    test_operand<byte> test_val(1);

    // Act
    rotation::rlc_operation(cpu, test_val);

    // Assert
    // 1 rlc (carry = 1) = 3
    ASSERT_EQ(test_val.value, 3);
    ASSERT_EQ(cpu.F.carry(), false);
}


TEST(ROTATION, RR_CARRY_ON) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(1);

    // Act
    rotation::rr_operation(cpu, test_val);

    // Assert
    // 1 rr == 128
    ASSERT_EQ(test_val.value, 128);
    ASSERT_EQ(cpu.F.carry(), true);
}

TEST(ROTATION, RR_CARRY_OFF){
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    cpu.F.carry(true);
    test_operand<byte> test_val(128);

    // Act
    rotation::rr_operation(cpu, test_val);
    
    // Assert
    // 128 rr == 64
    ASSERT_EQ(test_val.value, 64);
    ASSERT_EQ(cpu.F.carry(), false);
}

TEST(ROTATION, RRC_CARRY_OFF) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(1);

    // Act
    rotation::rrc_operation(cpu, test_val);
    
    // Assert
    // 1 rrc (carry = 0) == 0
    ASSERT_EQ(test_val.value, 0);
    ASSERT_EQ(cpu.F.carry(), true);

}

TEST(ROTATION, RRC_CARRY_ON) {
    
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    cpu.F.carry(true);
    test_operand<byte> test_val(128);

    // Act
    rotation::rrc_operation(cpu, test_val);

    // Assert
    // 128 rrc (carry = 1) = 192
    ASSERT_EQ(test_val.value, 192);
    ASSERT_EQ(cpu.F.carry(), false);
}

TEST(ROTATION, SLA_TURN_CARRY_TRUE) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(128);
    
    // Act
    rotation::sla_operation(cpu, test_val);
    
    // Assert
    // 128 sla == 0
    ASSERT_EQ(test_val.value, 0);
    ASSERT_EQ(cpu.F.carry(), true);
}

TEST(ROTATION, SLA_TURN_CARRY_OFF) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    cpu.F.carry(true);
    test_operand<byte> test_val(64);

    // Act
    rotation::sla_operation(cpu, test_val);
    
    // Assert
    // 64 sla == 128
    ASSERT_EQ(test_val.value, 128);
    ASSERT_EQ(cpu.F.carry(), false);
}

TEST(ROTATION, SRA_TURN_CARRY_ON) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(1);

    // Act
    rotation::sra_operation(cpu, test_val);

    // Assert
    // 1 sra = 0
    ASSERT_EQ(test_val.value, 0);
    ASSERT_EQ(cpu.F.carry(), true);
}

TEST(ROTATION, SRA_KEEP_SIGNED_VALUE) {
    
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    cpu.F.carry(false);
    test_operand<byte> test_val(0b10000000);
    
    // Act
    rotation::sra_operation(cpu, test_val);

    // -128 sra = -64
    ASSERT_EQ(test_val.value, 0b11000000); // -64
    ASSERT_EQ(cpu.F.carry(), false);
}

TEST(ROTATION, SRL_TURN_CARRY_ON) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(1);

    // Act
    rotation::srl_operation(cpu, test_val);

    // Assert
    // 1 srl = 0
    ASSERT_EQ(test_val.value, 0);
    ASSERT_EQ(cpu.F.carry(), true);
}

TEST(ROTATION, SRL_CHANGE_SIGNED) {
    // Arrange
    gamekid::system sys;
    gamekid::cpu::cpu& cpu = sys.cpu();
    test_operand<byte> test_val(128);
    
    // Act
    rotation::srl_operation(cpu, test_val);

    // Assert
    // 128 srl = 64
    ASSERT_EQ(test_val.value, 64);
    ASSERT_EQ(cpu.F.carry(), false);
}